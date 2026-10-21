#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "Device.h"
#include "SwapChain.h"

class FrameSynchronizer
{
public:

	void createSyncObjects(Device const& device, SwapChain const& swapChain);
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	uint32_t const& GetFrameIndex() const { return frameIndex; }
	static constexpr int GetMaxFramesInFlight() { return MAX_FRAMES_IN_FLIGHT; }
	vk::raii::Fence const& GetInFlightFence(uint32_t index) const { return inFlightFences.at(index); }
	vk::raii::Semaphore const& GetPresentCompleteSemaphore(uint32_t index) const { return presentCompleteSemaphores.at(index); }
	vk::raii::Semaphore const& GetRenderFinishedSemaphore(uint32_t index) const { return renderFinishedSemaphores.at(index); }
	void SetFramebufferResized(bool value) { framebufferResized = value; }

	bool IsFramebufferResized() const { return framebufferResized; }
	void AdvanceFrame() { frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT; }

private:
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;
	uint32_t frameIndex = 0;

	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
	bool framebufferResized = false;


};

