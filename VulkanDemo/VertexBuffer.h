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
	void vreateVertexBuffer();

	vk::raii::Buffer const& GetVertexBuffer() const { return vertexBuffer; }

private:
	Device device;
	GraphicPipeline pipeline;
	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory vertexBufferMemory = nullptr;

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
};

