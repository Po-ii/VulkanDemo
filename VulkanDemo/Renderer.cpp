#include "Renderer.h"

void Renderer::Initialize(GLFWwindow* window) 
{
	instance.createInstance();
	surface.createSurface(window, instance);
	device.GetPhysicalDevice();
	device.createLogicalDevice(surface);
	swapChain.createSwapChain(window, device, surface);
	swapChain.createImageViews(device);
	pipeline.createGraphicPipeline(device, swapChain);
	commands.createCommandPool(device);
	commands.createCommandBuffers(device, synchronization);
	synchronization.createSyncObjects(device, swapChain);
}

void Renderer::drawFrame(GLFWwindow* window)
{
	auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}

	auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		recreateSwapChain(window);
		return;
	}
	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	device.resetFences(*inFlightFences[frameIndex]);
	commandBuffers[frameIndex].reset();
	recordCommandBuffer(imageIndex);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo   submitInfo
	{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
		.pWaitDstStageMask = &waitDestinationStageMask,
		.commandBufferCount = 1,
		.pCommandBuffers = &*commandBuffers[frameIndex],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &*renderFinishedSemaphores[imageIndex]
	};
	queue.submit(submitInfo, *inFlightFences[frameIndex]);

	const vk::PresentInfoKHR presentInfoKHR
	{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*renderFinishedSemaphores[imageIndex],
		.swapchainCount = 1,
		.pSwapchains = &*swapChain,
		.pImageIndices = &imageIndex
	};
	try
	{
		result = queue.presentKHR(presentInfoKHR);
	}
	catch (const vk::OutOfDateKHRError&)
	{
		framebufferResized = false;
		recreateSwapChain(window);
		return;
	}
	if (result == vk::Result::eSuboptimalKHR || framebufferResized)
	{
		framebufferResized = false;
		recreateSwapChain(window);
	}
	else
	{
		assert(result == vk::Result::eSuccess);
	}

	frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::waitIdle(Device const& device) 
{
	return device.GetDevice().waitIdle();
}
