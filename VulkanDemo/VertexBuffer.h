#pragma once
#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "Device.h"
#include "GraphicPipeline.h"


class VertexBuffer
{
public:
	void createVertexBuffer(Device const& device, GraphicPipeline const& pipeline);

	vk::raii::Buffer const& GetVertexBuffer() const { return vertexBuffer; }

private:
	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory vertexBufferMemory = nullptr;

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, Device const& device);
};

