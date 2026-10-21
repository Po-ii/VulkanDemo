#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "Instance.h"
#include "Surface.h"

class Device
{
public:

	void pickPhysicalDevice(Instance const& instance);
	void createLogicalDevice(Surface const& surface);

	vk::raii::Device const& GetDevice() const { return device; }
	vk::raii::PhysicalDevice const& GetPhysicalDevice() const { return physicalDevice; }
	vk::raii::Queue const& GetQueue() const { return queue; }
	uint32_t GetQueueIndex() const { return queueIndex; }

private: 
	vk::raii::Device device = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Queue queue = nullptr;
	uint32_t queueIndex = ~0;

	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
};

