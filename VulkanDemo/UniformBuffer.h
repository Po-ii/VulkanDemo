#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <cstdint>

class Device;
class SwapChain;
class GraphicPipeline;
class VertexBuffer;
class FrameSynchronizer;

class UniformBuffer
{
public:
	void createUniformBuffer(Device const& device, GraphicPipeline const& pipeline, FrameSynchronizer const& sync, VertexBuffer vertBuffer);
	void updateUniformBuffer(uint32_t currentImage, SwapChain const& swapChain);

private:
	std::vector<vk::raii::Buffer>       uniformBuffers;
	std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
	std::vector<void*>                 uniformBuffersMapped;

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
};

