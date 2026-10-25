#pragma once
#define NOMINMAX
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>

class Device;
class Surface;
class Texture;
class DepthBuffer;

class SwapChain
{
public:
	void createSwapChain(GLFWwindow* window, Device const& device, Surface const& surface);
	void createImageViews(Device const& device, Texture const& texture);
	void cleanupSwapChain();
	void recreateSwapChain(GLFWwindow* window, Device const& device, Surface const& surface, Texture const& texture);

	vk::raii::SwapchainKHR const& GetSwapChain() const { return swapChain; }
	vk::Extent2D GetSwapChainExtent() const { return swapChainExtent; }
	vk::SurfaceFormatKHR const& GetSwapChainSurfaceFormat() const { return swapChainSurfaceFormat; }
	std::vector<vk::Image> GetSwapChainImages() const { return swapChainImages; }
	std::vector<vk::raii::ImageView> const& GetSwapChainImageViews() const { return swapChainImageViews; }
	
private:
	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	vk::SurfaceFormatKHR swapChainSurfaceFormat;
	vk::Extent2D swapChainExtent;
	std::vector<vk::raii::ImageView> swapChainImageViews;

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities, GLFWwindow* window);
	uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);
};

