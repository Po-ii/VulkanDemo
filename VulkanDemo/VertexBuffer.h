#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"

class Device;
class GraphicPipeline;
class CommandManager;

class VertexBuffer
{
public:
	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, Device const& device, GraphicPipeline const& pipeline);
	void createVertexBuffer(Device const& device, GraphicPipeline const& pipeline, CommandManager const& commands);
	void createIndexBuffer(Device const& device, GraphicPipeline const& pipeline, CommandManager const& commands);

	vk::raii::Buffer const& GetVertexBuffer() const { return vertexBuffer; }
	vk::raii::Buffer const& GetIndexBuffer() const { return indexBuffer; }

private:
	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory VertexBufferMemory = nullptr;
	vk::raii::Buffer indexBuffer = nullptr;
	vk::raii::DeviceMemory indexBufferMemory = nullptr;

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, Device const& device);
	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size, Device const& device, CommandManager const& commands);
};

