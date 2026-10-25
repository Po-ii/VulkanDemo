#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"

class Device;
class GraphicPipeline;
class UniformBuffer;
class FrameSynchronizer;

class DescriptorManager
{
public:
	void createDescriptorPool(Device const& device, FrameSynchronizer const& sync);
	void createDescriptorSets(Device const& device, GraphicPipeline const& pipeline, UniformBuffer const& uniformBuffer, FrameSynchronizer const& sync);

	vk::raii::DescriptorSet const& GetDescriptorSet(uint32_t frameIndex) const { return descriptorSets.at(frameIndex); }

private:
	vk::raii::DescriptorPool descriptorPool = nullptr;
	std::vector<vk::raii::DescriptorSet> descriptorSets;
};

