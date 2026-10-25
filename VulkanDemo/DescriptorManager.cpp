#include "DescriptorManager.h"
#include "Device.h"
#include "GraphicPipeline.h"
#include "FrameSynchronizer.h"
#include "UniformBuffer.h"
#include "Texture.h"

void DescriptorManager::createDescriptorPool(Device const& device, FrameSynchronizer const& sync)
{
	std::array<vk::DescriptorPoolSize, 2> poolSize{ {{.type = vk::DescriptorType::eUniformBuffer, .descriptorCount = sync.GetMaxFramesInFlight()},
												{.type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = sync.GetMaxFramesInFlight()}} };
	vk::DescriptorPoolCreateInfo poolInfo
	{ 
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = sync.GetMaxFramesInFlight(),
		.poolSizeCount = static_cast<uint32_t>(poolSize.size()),
		.pPoolSizes = poolSize.data() 
	};
	descriptorPool = vk::raii::DescriptorPool(device.GetDevice(), poolInfo);
}

void DescriptorManager::createDescriptorSets(Device const& device, GraphicPipeline const& pipeline, UniformBuffer const& uniformBuffer, FrameSynchronizer const& sync, Texture const& texture)
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
		vk::DescriptorImageInfo  imageInfo{ .sampler = texture.GetTextureSampler(), .imageView = texture.GetTextureImageView(), .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal};
		std::array<vk::WriteDescriptorSet, 2> descriptorWrites
		{ 
			{{.dstSet = descriptorSets[i],
			  .dstBinding = 0,
			  .dstArrayElement = 0,
			  .descriptorCount = 1,
			  .descriptorType = vk::DescriptorType::eUniformBuffer,
			  .pBufferInfo = &bufferInfo},
			 {.dstSet = descriptorSets[i],
			  .dstBinding = 1,
			  .dstArrayElement = 0,
			  .descriptorCount = 1,
			  .descriptorType = vk::DescriptorType::eCombinedImageSampler,
			  .pImageInfo = &imageInfo}} 
		};
		device.GetDevice().updateDescriptorSets(descriptorWrites, {});
	}
}


