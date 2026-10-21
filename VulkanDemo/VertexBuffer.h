#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "Device.h"
#include "SwapChain.h"
#include "GraphicPipeline.h"
#include "CommandManager.h"

class VertexBuffer
{
public:
	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, Device const& device, GraphicPipeline const& pipeline);
	void createVertexBuffer(Device const& device, GraphicPipeline const& pipeline, CommandManager const& commands);

	vk::raii::Buffer const& GetVertexBuffer() const { return vertexBuffer; }

private:
	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory VertexBufferMemory = nullptr;

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, Device const& device);
	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size, Device const& device, CommandManager const& commands);
};

