#include "CommandManager.h"
#include "Device.h"
#include "SwapChain.h"
#include "GraphicPipeline.h"
#include "FrameSynchronizer.h"
#include "VertexBuffer.h"
#include "DescriptorManager.h"
#include "DepthBuffer.h"

void CommandManager::createCommandPool(Device const& device)
{
	vk::CommandPoolCreateInfo poolInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer, .queueFamilyIndex = device.GetQueueIndex()};
	commandPool = vk::raii::CommandPool(device.GetDevice(), poolInfo);
}

void CommandManager::createCommandBuffers(Device const& device, FrameSynchronizer const& synchronizer)
{
	vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = synchronizer.GetMaxFramesInFlight()};
	commandBuffers = vk::raii::CommandBuffers(device.GetDevice(), allocInfo);
}

void CommandManager::recordCommandBuffer(
	uint32_t imageIndex, SwapChain const& swapChain, FrameSynchronizer const& synchronizer, GraphicPipeline const& pipeline, VertexBuffer const& vertexBuffer, DescriptorManager const& descriptors, DepthBuffer const& depthBuffer)
{
	auto& commandBuffer = commandBuffers[synchronizer.GetFrameIndex()];
	commandBuffer.begin({});
	transition_image_layout(
		swapChain.GetSwapChainImages()[imageIndex],
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::ImageAspectFlagBits::eColor,
		swapChain,
		synchronizer);

	transition_image_layout(
		*depthBuffer.GetDepthImage(),
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthAttachmentOptimal,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::ImageAspectFlagBits::eDepth,
		swapChain,
		synchronizer);

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

	vk::RenderingAttachmentInfo colorAttachmentInfo = {
		.imageView = swapChain.GetSwapChainImageViews()[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor };

	vk::RenderingAttachmentInfo depthAttachmentInfo = {
		.imageView = depthBuffer.GetDepthImageView(),
		.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eDontCare,
		.clearValue = clearDepth };

	vk::RenderingInfo renderingInfo = {
		.renderArea = {.offset = {0, 0}, .extent = swapChain.GetSwapChainExtent()},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentInfo,
		.pDepthAttachment = &depthAttachmentInfo };

	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.GetGraphicPipeline());
	commandBuffer.setViewport(0, vk::Viewport(0.0f, static_cast<float>(swapChain.GetSwapChainExtent().height), static_cast<float>(swapChain.GetSwapChainExtent().width), -static_cast<float>(swapChain.GetSwapChainExtent().height), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChain.GetSwapChainExtent()));
	commandBuffer.bindVertexBuffers(0, *vertexBuffer.GetVertexBuffer(), {0});
	commandBuffer.bindIndexBuffer(*vertexBuffer.GetIndexBuffer(), 0, vk::IndexTypeValue<decltype(pipeline.GetIndices())::value_type>::value);
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.GetPipelineLayout(), 0, *descriptors.GetDescriptorSet(synchronizer.GetFrameIndex()), nullptr);
	commandBuffer.drawIndexed(static_cast<uint32_t>(pipeline.GetIndices().size()), 1, 0, 0, 0);
	commandBuffer.endRendering();

	transition_image_layout(
		swapChain.GetSwapChainImages()[imageIndex],
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		{},
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eBottomOfPipe,
		vk::ImageAspectFlagBits::eColor,
		swapChain,
		synchronizer);
	commandBuffer.end();
}

vk::raii::CommandBuffer CommandManager::beginSingleTimeCommands(Device const& device) const
{
	vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
	vk::raii::CommandBuffer       commandBuffer = std::move(vk::raii::CommandBuffers(device.GetDevice(), allocInfo).front());

	vk::CommandBufferBeginInfo beginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
	commandBuffer.begin(beginInfo);

	return std::move(commandBuffer);
}

void CommandManager::endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer, Device const& device) const
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
	device.GetQueue().submit(submitInfo, nullptr);
	device.GetQueue().waitIdle();
}

void CommandManager::transition_image_layout(
	vk::Image image, vk::ImageLayout old_layout, 
	vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask, 
	vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask,
	vk::PipelineStageFlags2 dst_stage_mask, vk::ImageAspectFlags image_aspect_flags, 
	SwapChain const& swapChain, FrameSynchronizer const& synchronizer)
{
	vk::ImageMemoryBarrier2 barrier = {
			.srcStageMask = src_stage_mask,
			.srcAccessMask = src_access_mask,
			.dstStageMask = dst_stage_mask,
			.dstAccessMask = dst_access_mask,
			.oldLayout = old_layout,
			.newLayout = new_layout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = image,
			.subresourceRange = {
				   .aspectMask = vk::ImageAspectFlagBits::eColor,
				   .baseMipLevel = 0,
				   .levelCount = 1,
				   .baseArrayLayer = 0,
				   .layerCount = 1} };
	vk::DependencyInfo dependency_info = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier };
	commandBuffers[synchronizer.GetFrameIndex()].pipelineBarrier2(dependency_info);
}
