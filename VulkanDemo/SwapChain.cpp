#include "SwapChain.h"
#include "Device.h"
#include "Surface.h"
#include "Texture.h"

void SwapChain::createSwapChain(GLFWwindow* window, Device const& device, Surface const& surface)
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = device.GetPhysicalDevice().getSurfaceCapabilitiesKHR(*surface.GetSurface());
	swapChainExtent = chooseSwapExtent(surfaceCapabilities, window);
	uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

	std::vector<vk::SurfaceFormatKHR> availableFormats = device.GetPhysicalDevice().getSurfaceFormatsKHR(*surface.GetSurface());
	swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

	std::vector<vk::PresentModeKHR> availablePresentModes = device.GetPhysicalDevice().getSurfacePresentModesKHR(*surface.GetSurface());
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(availablePresentModes);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo
	{
		.surface = *surface.GetSurface(),
		.minImageCount = minImageCount,
		.imageFormat = swapChainSurfaceFormat.format,
		.imageColorSpace = swapChainSurfaceFormat.colorSpace,
		.imageExtent = swapChainExtent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = presentMode,
		.clipped = true
	};

	swapChain = vk::raii::SwapchainKHR(device.GetDevice(), swapChainCreateInfo);
	swapChainImages = swapChain.getImages();
}

void SwapChain::createImageViews(Device const& device, Texture const& texture)
{
	assert(swapChainImageViews.empty());

	swapChainImageViews.reserve(swapChainImages.size());
	for (auto& image : swapChainImages)
	{
		swapChainImageViews.emplace_back(texture.createImageView(image, swapChainSurfaceFormat.format, device));
	}
}

void SwapChain::cleanupSwapChain()
{
	swapChainImageViews.clear();
	swapChain = nullptr;
}

vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
{
	const auto formatIt = std::ranges::find_if(availableFormats, [](const auto& format)
		{
			return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
		});
	return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR SwapChain::chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
{
	assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) {return presentMode == vk::PresentModeKHR::eFifo;}));
	return std::ranges::any_of(availablePresentModes,
		[](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChain::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities, GLFWwindow* window)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	return
	{
		std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	};
}

uint32_t SwapChain::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
	auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
	if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
	{
		minImageCount = surfaceCapabilities.maxImageCount;
	}
	return minImageCount;
}

void SwapChain::recreateSwapChain(GLFWwindow* window, Device const& device, Surface const& surface, Texture const& texture)
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while ((width == 0 || height == 0) && !glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}
	if (glfwWindowShouldClose(window))
	{
		return;
	}

	device.GetDevice().waitIdle();
	cleanupSwapChain();
	createSwapChain(window, device, surface);
	createImageViews(device, texture);
}
