#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <array>
#include "Device.h"
#include "SwapChain.h"


class GraphicPipeline
{
private:
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicPipeline = nullptr;

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, Device const& device) const;
	static std::vector<char> readFile(const std::string& filename);

	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static vk::VertexInputBindingDescription getBindingDescription()
		{
			return { .binding = 0, .stride = sizeof(Vertex), . inputRate = vk::VertexInputRate::eVertex };
		}

		static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			return { {{.location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, pos)},
			{.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)}} };
		}
	};

	const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };

public:
	void createGraphicPipeline(Device const& device, SwapChain const& swapChain);

	vk::raii::Pipeline const& GetGraphicPipeline() const { return graphicPipeline; }
	const std::vector<Vertex>GetVertices() const { return vertices; }
};

