#include "DescriptorManager.h"
#include "Device.h"
#include "GraphicPipeline.h"
#include "FrameSynchronizer.h"
#include "UniformBuffer.h"

void DescriptorManager::createDescriptorPool(Device const& device, FrameSynchronizer const& sync)
{
	vk::DescriptorPoolSize poolSize{ .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = sync.GetMaxFramesInFlight()};
	vk::DescriptorPoolCreateInfo poolInfo{ .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, .maxSets = sync.GetMaxFramesInFlight(), .poolSizeCount = 1, .pPoolSizes = &poolSize};
	descriptorPool = vk::raii::DescriptorPool(device.GetDevice(), poolInfo);
}

void DescriptorManager::createDescriptorSets(Device const& device, GraphicPipeline const& pipeline, UniformBuffer const& uniformBuffer, FrameSynchronizer const& sync)
{
	std::vector<vk::DescriptorSetLayout> layouts(sync.GetMaxFramesInFlight(), *pipeline.GetDescriptorSetLayout());
	vk::DescriptorSetAllocateInfo allocInfo
	{ 
		.descriptorPool = descriptorPool,
		.descriptorSetCount = static_cast<uint32_t>(layouts.size()),
		.pSetLayouts = layouts.data() 
	};
	descriptorSets = device.GetDevice().allocateDescriptorSets(allocInfo);

	for (size_t i = 0; i < sync.GetMaxFramesInFlight(); i++)
	{
		vk::DescriptorBufferInfo bufferInfo{ .buffer = uniformBuffer.GetUniformBuffer(i), .offset = 0, .range = sizeof(UniformBufferObject) };
		vk::WriteDescriptorSet descriptorWrite
		{
			.dstSet = descriptorSets[i],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.pBufferInfo = &bufferInfo
		};
		device.GetDevice().updateDescriptorSets(descriptorWrite, {});
	}
}


