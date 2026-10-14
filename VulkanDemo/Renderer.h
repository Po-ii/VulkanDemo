#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

class Renderer
{
public:
	void CreateInstance();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSurface(GLFWwindow* window);
	void createSwapChain();

private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessanger = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::PhysicalDeviceFeatures deviceFeatures;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	
	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };
	std::vector<char const*>GetRequiredExtensions();
	std::vector<char const*>GetRequiredLayers();
	std::vector<const char*>GetRequiredDeviceExtensions();

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities);

	void setupDebugMessenger();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT,
		vk::DebugUtilsMessageTypeFlagsEXT,
		const vk::DebugUtilsMessengerCallbackDataEXT*,
		void*);
};

