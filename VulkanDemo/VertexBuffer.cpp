#include "VertexBuffer.h"
#include "Device.h"
#include "GraphicPipeline.h"
#include "CommandManager.h"

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> VertexBuffer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, Device const& device ,GraphicPipeline const& pipeline)
{
	vk::BufferCreateInfo bufferInfo
	{
		.size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};
	vk::raii::Buffer buffer = vk::raii::Buffer(device.GetDevice(), bufferInfo);

	vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo
	{
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, device)
	};
	vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(device.GetDevice(), allocInfo);
	buffer.bindMemory(*bufferMemory, 0);
	return { std::move(buffer), std::move(bufferMemory) };
}

void VertexBuffer::createVertexBuffer(Device const& device, GraphicPipeline const& pipeline, CommandManager const& commands)
{
	vk::DeviceSize bufferSize = sizeof(pipeline.GetVertices()[0]) * pipeline.GetVertices().size();

	auto [stagingBuffer, stagingBufferMemory] = createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, device, pipeline);
	void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(dataStaging, pipeline.GetVertices().data(), bufferSize);
	stagingBufferMemory.unmapMemory();

	std::tie(vertexBuffer, VertexBufferMemory) = createBuffer(bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, device, pipeline);
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize, device, commands);
}

void VertexBuffer::createIndexBuffer(Device const& device, GraphicPipeline const& pipeline, CommandManager const& commands)
{
	vk::DeviceSize bufferSize = sizeof(pipeline.GetIndices()[0]) * pipeline.GetIndices().size();

	auto [stagingBuffer, stagingBufferMemory] =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, device, pipeline);

	void* data = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(data, pipeline.GetIndices().data(), (size_t)bufferSize);
	stagingBufferMemory.unmapMemory();

	std::tie(indexBuffer, indexBufferMemory) =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, device, pipeline);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize, device, commands);
}

uint32_t VertexBuffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, Device const& device)
{
	vk::PhysicalDeviceMemoryProperties memProperties = device.GetPhysicalDevice().getMemoryProperties();
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

void VertexBuffer::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size, Device const& device, CommandManager const& commands)
{
	vk::CommandBufferAllocateInfo allocInfo { .commandPool = commands.GetCommandPool(), .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1};
	vk::raii::CommandBuffer commandCopyBuffer = std::move(device.GetDevice().allocateCommandBuffers(allocInfo).front());

	commandCopyBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
	commandCopyBuffer.end();
	device.GetQueue().submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer}, nullptr);
	device.GetQueue().waitIdle();
}
