#include <iostream>
#include "Renderer.h"
#include "Debug.h"

void Renderer::CreateInstance()
{
	vk::ApplicationInfo appInfo
	{
		.pApplicationName = "Vulkan Triangle",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Gaming Campus Triangle",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = vk::ApiVersion14
	};

	std::vector requiredExtensions = GetRequiredExtensions();
	auto extensionProperties = context.enumerateInstanceExtensionProperties();

	for (uint32_t i = 0; i < requiredExtensions.size(); i++)
	{
		auto predicate = [extension = requiredExtensions[i]](auto const& extensionProperty)
		{
			return strcmp(extensionProperty.extensionName, extension) == 0;
		};

		if (std::ranges::none_of(extensionProperties, predicate))
		{
			throw std::runtime_error("Required GLFW extension not supported " + std::string(requiredExtensions[i]));
		}
	}

	std::vector requiredLayers = GetRequiredLayers();
	auto layerProperties = context.enumerateInstanceLayerProperties();
	auto predicate_layer = [&layerProperties](auto const& requiredLayer)
	{
		return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty)
		{
			return strcmp(layerProperty.layerName, requiredLayer) == 0;
		});
	};

	auto unsupportedLayerIt = std::ranges::find_if(requiredLayers, predicate_layer);
	if (unsupportedLayerIt != requiredLayers.end())
	{
		throw std::runtime_error("Required Layer not supported " + std::string(*unsupportedLayerIt));
	}

	vk::InstanceCreateInfo createInfo
	{
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()
	};

	instance = vk::raii::Instance(context, createInfo);
	setupDebugMessanger();
}

void Renderer::pickPhysicalDevice()
{
	auto physicalDevices = instance.enumeratePhysicalDevices();
	if (physicalDevices.empty())
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
}

std::vector<char const*> Renderer::GetRequiredExtensions()
{
	uint32_t glfwExtensionsCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

	std::vector<char const*> applicationExtension = { vk::EXTDebugUtilsExtensionName };

	std::vector requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
	requiredExtensions.append_range(applicationExtension);

	return requiredExtensions;
}

std::vector<char const*> Renderer::GetRequiredLayers()
{
	const std::vector validationLayers = { "VK_LAYER_KHRONOS_validation" };

	std::vector<char const*> requiredLayers = {};

	if (Debug::enableDebug)
		requiredLayers.append_range(validationLayers);

	return requiredLayers;
}

void Renderer::setupDebugMessanger()
{
	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	vk::DebugUtilsMessageTypeFlagsEXT TypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

	vk::DebugUtilsMessengerCreateInfoEXT createInfo
	{
		.messageSeverity = severityFlags,
		.messageType = TypeFlags,
		.pfnUserCallback = &DebugCallback,
	};

	debugMessanger = instance.createDebugUtilsMessengerEXT(createInfo);
}

bool Renderer::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
{
	bool supportsVulkab14 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion14;

	auto deviceProperties = physicalDevice.getProperties();
	auto deviceFeatures = physicalDevice.getFeatures();
	if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && deviceFeatures.geometryShader)
	{
		return true;
	}

	return false;
}

vk::Bool32 Renderer::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* data, void*)
{
	std::println(std::cout, "Validation Layer : Type [{}]", vk::to_string(type));
	std::println(std::cout, "Message : {}", data->pMessage);

	return vk::False;
}



