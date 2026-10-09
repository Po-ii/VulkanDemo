#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>

class Renderer
{
public:
	void CreateInstance();

private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessanger = nullptr;
	
	std::vector<char const*>GetRequiredExtensions();
	std::vector<char const*>GetRequiredLayers();

	void setupDebugMessanger();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT,
		vk::DebugUtilsMessageTypeFlagsEXT,
		const vk::DebugUtilsMessengerCallbackDataEXT*,
		void*);
};

