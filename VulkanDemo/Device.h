#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"

class Instance;
class Surface;

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

