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
#include "GraphicPipeline.h"
#include "FrameSynchronizer.h"
#include "VertexBuffer.h"

class CommandManager
{
public:
	void createCommandPool(Device const& device);
	void createCommandBuffers(Device const& device, FrameSynchronizer const& synchronizer);
	void recordCommandBuffer(uint32_t imageIndex, SwapChain const& swapChain, FrameSynchronizer const& synchronizer, GraphicPipeline const& pipeline, VertexBuffer const& verBuffer);
	
	vk::raii::CommandBuffer const& GetCommandBuffer(uint32_t index) const { return commandBuffers.at(index); }
	vk::raii::CommandBuffer& GetCommandBuffer(uint32_t index) { return commandBuffers.at(index); }

private:
	vk::raii::CommandPool commandPool = nullptr;
	std::vector<vk::raii::CommandBuffer> commandBuffers;

	void transition_image_layout(
		uint32_t imageIndex,
		vk::ImageLayout  old_layout,
		vk::ImageLayout  new_layout,
		vk::AccessFlags2 src_access_mask,
		vk::AccessFlags2 dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask,
		SwapChain const& swapChain,
		FrameSynchronizer const& synchronizer);
};

