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

class GraphicPipeline
{
public:
	void createGraphicPipeline(Device const& device, SwapChain const& swapChain);

	vk::raii::Pipeline const& GetGraphicPipeline() const { return graphicPipeline; }
private:
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicPipeline = nullptr;

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, Device const& device) const;
	static std::vector<char> readFile(const std::string& filename);
};

