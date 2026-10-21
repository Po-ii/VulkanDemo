#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "Instance.h"

class Surface
{
public:
	void createSurface(GLFWwindow* window, Instance const& instance);
	vk::raii::SurfaceKHR const& GetSurface() const { return surface; }

private:
	vk::raii::SurfaceKHR surface = nullptr;
};

