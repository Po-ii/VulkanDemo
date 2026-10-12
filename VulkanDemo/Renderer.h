#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>

class Renderer
{
public:
	void CreateInstance();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSurface();

private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessanger = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::PhysicalDeviceFeatures deviceFeatures;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	
	std::vector<char const*>GetRequiredExtensions();
	std::vector<char const*>GetRequiredLayers();
	std::vector<const char*>GetRequiredDeviceExtensions();

	void setupDebugMessenger();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT,
		vk::DebugUtilsMessageTypeFlagsEXT,
		const vk::DebugUtilsMessengerCallbackDataEXT*,
		void*);
};

