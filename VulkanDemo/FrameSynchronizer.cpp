#include "FrameSynchronizer.h"
#include "Device.h"
#include "SwapChain.h"

void FrameSynchronizer::createSyncObjects(Device const& device, SwapChain const& swapChain)
{
	assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());
	for (size_t i = 0; i < swapChain.GetSwapChainImages().size(); i++)
	{
		renderFinishedSemaphores.emplace_back(device.GetDevice(), vk::SemaphoreCreateInfo());
	}
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		presentCompleteSemaphores.emplace_back(device.GetDevice(), vk::SemaphoreCreateInfo());
		inFlightFences.emplace_back(device.GetDevice(), vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
	}
}