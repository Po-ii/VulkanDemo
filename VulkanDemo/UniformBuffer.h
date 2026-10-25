#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Device;
class SwapChain;
class GraphicPipeline;
class VertexBuffer;
class FrameSynchronizer;

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class UniformBuffer
{
public:
	void createUniformBuffer(Device const& device, GraphicPipeline const& pipeline, FrameSynchronizer const& sync, VertexBuffer const& vertBuffer);
	void updateUniformBuffer(uint32_t currentImage, SwapChain const& swapChain);

	vk::raii::Buffer const& GetUniformBuffer(uint32_t index) const { return uniformBuffers.at(index); }

private:
	std::vector<vk::raii::Buffer> uniformBuffers;
	std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;
};

