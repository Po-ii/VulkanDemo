#pragma once
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "Instance.h"
#include "Surface.h"
#include "Device.h"
#include "SwapChain.h"
#include "GraphicPipeline.h"
#include "CommandManager.h"
#include "FrameSynchronizer.h"
#include "VertexBuffer.h"


class Renderer
{
public:
	void Initialize(GLFWwindow* window);
	void drawFrame(GLFWwindow* window);
	void waitIdle();
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
	Instance instance;
	Device device;
	Surface surface;
	SwapChain swapChain;
	GraphicPipeline pipeline;
	CommandManager commands;
	FrameSynchronizer synchronization;
	VertexBuffer verBuffer;
	
};

