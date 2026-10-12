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
	std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
	auto const devIter = std::ranges::find_if(physicalDevices, [this](auto const& physicalDevice)
		{
			return isDeviceSuitable(physicalDevice);
		});
	if (devIter == physicalDevices.end())
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	physicalDevice = *devIter;
}

void Renderer::createLogicalDevice()
{
	float queuePriority = 0.5f;
	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
	auto graphicsQueueFamilyProperty = std::ranges::find_if(queueFamilyProperties, [](auto const& qfp)
		{
			return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
		});

	auto graphicIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo
	{
		.queueFamilyIndex = graphicIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	vk::StructureChain<vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan11Features,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
		featureChain =
	{
		{},
		{.shaderDrawParameters = true},
		{.dynamicRendering = true},
		{.extendedDynamicState = true}
	};

	vk::DeviceCreateInfo deviceCreateInfo
	{
		.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledLayerCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
		.ppEnabledExtensionNames = requiredDeviceExtension.data()
	};

	device = vk::raii::Device(physicalDevice, deviceCreateInfo);
	graphicsQueue = vk::raii::Queue(device, graphicIndex, 0);
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

/*std::vector<const char*> Renderer::GetRequiredDeviceExtensions()
//{
//	return { vk::KHRSwapchainExtensionName };
}*/

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
	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };
	bool supportsVulkan13 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

	auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp)
		{
			return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
		});

	auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	bool supportsAllRequiredExtensions = std::ranges::all_of(requiredDeviceExtension, [&availableExtensions](auto const& requiredDeviceExtension)
		{
			return std::ranges::any_of(availableExtensions, [&requiredDeviceExtension](auto const& availableDeviceExtension)
				{
					return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0;
				});
		});

	auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan11Features,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
	bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
		features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
		features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

	return supportsVulkan13 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}

vk::Bool32 Renderer::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* data, void*)
{
	std::println(std::cout, "Validation Layer : Type [{}]", vk::to_string(type));
	std::println(std::cout, "Message : {}", data->pMessage);

	return vk::False;
}
