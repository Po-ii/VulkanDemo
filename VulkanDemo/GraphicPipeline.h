#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <glm/glm.hpp>
#include <array>

class Device;
class SwapChain;

class GraphicPipeline
{
private:
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
	vk::raii::Pipeline graphicPipeline = nullptr;

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, Device const& device) const;
	static std::vector<char> readFile(const std::string& filename);

	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static vk::VertexInputBindingDescription getBindingDescription()
		{
			return { .binding = 0, .stride = sizeof(Vertex), .inputRate = vk::VertexInputRate::eVertex };
		}

		static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			return { {{.location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, pos)},
			{.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)}} };
		}
	};

	const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
	};

public:
	void createGraphicPipeline(Device const& device, SwapChain const& swapChain);
	void createDescriptorSetLayout(Device const& device);

	vk::raii::Pipeline const& GetGraphicPipeline() const { return graphicPipeline; }
	const std::vector<Vertex>GetVertices() const { return vertices; }
	const std::vector<uint16_t>GetIndices() const { return indices; }
	vk::raii::DescriptorSetLayout const& GetDescriptorSetLayout() const { return descriptorSetLayout; }
	vk::raii::PipelineLayout const& GetPipelineLayout() const { return pipelineLayout; }
};

