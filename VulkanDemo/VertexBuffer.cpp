#include "VertexBuffer.h"

void VertexBuffer::createVertexBuffer(Device const& device, GraphicPipeline const& pipeline)
{
	vk::BufferCreateInfo bufferInfo
	{
		.size = sizeof(pipeline.GetVertices()[0]) * pipeline.GetVertices().size(),
		.usage = vk::BufferUsageFlagBits::eVertexBuffer,
		.sharingMode = vk::SharingMode::eExclusive
	};
	vertexBuffer = vk::raii::Buffer(device.GetDevice(), bufferInfo);

	vk::MemoryRequirements memRequirements = vertexBuffer.getMemoryRequirements();
	vk::MemoryAllocateInfo memoryAllocateInfo
	{
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, device)
	};
	vertexBufferMemory = vk::raii::DeviceMemory(device.GetDevice(), memoryAllocateInfo);
	vertexBuffer.bindMemory(*vertexBufferMemory, 0);

	void* data = vertexBufferMemory.mapMemory(0, bufferInfo.size);
	memcpy(data, pipeline.GetVertices().data(), bufferInfo.size);
	vertexBufferMemory.unmapMemory();
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
