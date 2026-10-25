#include "Renderer.h"

void Renderer::Initialize(GLFWwindow* window) 
{
	instance.createInstance();
	surface.createSurface(window, instance);
	device.pickPhysicalDevice(instance);
	device.createLogicalDevice(surface);
	swapChain.createSwapChain(window, device, surface);
	swapChain.createImageViews(device, texture);
	pipeline.createDescriptorSetLayout(device);
	pipeline.createGraphicPipeline(device, swapChain, depth);
	commands.createCommandPool(device);
	depth.createDepthResources(device, swapChain, verBuffer, texture);
	texture.createTextureImage(device, pipeline, verBuffer, commands);
	texture.createTextureImageViews(device);
	texture.createTexturSampler(device);
	verBuffer.createVertexBuffer(device, pipeline, commands);
	verBuffer.createIndexBuffer(device, pipeline, commands);
	uniformBuffer.createUniformBuffer(device, pipeline, synchronization, verBuffer);
	descriptors.createDescriptorPool(device, synchronization);
	descriptors.createDescriptorSets(device, pipeline, uniformBuffer, synchronization, texture);
	commands.createCommandBuffers(device, synchronization);
	synchronization.createSyncObjects(device, swapChain);
}

void Renderer::drawFrame(GLFWwindow* window)
{
	const uint32_t frameIndex = synchronization.GetFrameIndex();

	vk::raii::Fence const& inFlightFence = synchronization.GetInFlightFence(frameIndex);
	auto fenceResult = device.GetDevice().waitForFences(*inFlightFence, vk::True, UINT64_MAX);
	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}

	auto [result, imageIndex] = swapChain.GetSwapChain().acquireNextImage(UINT64_MAX, *synchronization.GetPresentCompleteSemaphore(frameIndex), nullptr);
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		swapChain.recreateSwapChain(window, device, surface, texture);
		return;
	}
	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	device.GetDevice().resetFences(*inFlightFence);
	commands.GetCommandBuffer(frameIndex).reset();
	uniformBuffer.updateUniformBuffer(frameIndex, swapChain);
	commands.recordCommandBuffer(imageIndex, swapChain, synchronization, pipeline, verBuffer, descriptors, depth);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo   submitInfo
	{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*synchronization.GetPresentCompleteSemaphore(frameIndex),
		.pWaitDstStageMask = &waitDestinationStageMask,
		.commandBufferCount = 1,
		.pCommandBuffers = &*commands.GetCommandBuffer(frameIndex),
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &*synchronization.GetRenderFinishedSemaphore(imageIndex)
	};
	device.GetQueue().submit(submitInfo, *inFlightFence);

	const vk::PresentInfoKHR presentInfoKHR
	{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*synchronization.GetRenderFinishedSemaphore(imageIndex),
		.swapchainCount = 1,
		.pSwapchains = &*swapChain.GetSwapChain(),
		.pImageIndices = &imageIndex
	};
	try
	{
		result = device.GetQueue().presentKHR(presentInfoKHR);
	}
	catch (const vk::OutOfDateKHRError&)
	{
		synchronization.SetFramebufferResized(false);
		swapChain.recreateSwapChain(window, device, surface, texture);
		return;
	}
	if (result == vk::Result::eSuboptimalKHR || synchronization.IsFramebufferResized() )
	{
		synchronization.SetFramebufferResized(false);
		swapChain.recreateSwapChain(window, device, surface, texture);
	}
	else
	{
		assert(result == vk::Result::eSuccess);
	}

	synchronization.AdvanceFrame();
}

void Renderer::waitIdle()
{
	return device.GetDevice().waitIdle();
}

void Renderer::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	renderer->synchronization.SetFramebufferResized(true);
}