#include "Application.h"

void Application::Init()
{
    InitWindow();
    InitVulkan();
}

void Application::Run()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        renderer.drawFrame(window);
    }
    renderer.waitIdle();
}

void Application::Destroy()
{
    DestroyVulkan();
    DestroyWindow();
}

void Application::InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, "VulkanDemo", nullptr, nullptr);
    glfwSetWindowUserPointer(window, &renderer);
    glfwSetFramebufferSizeCallback(window, renderer.framebufferResizeCallback);
}

void Application::InitVulkan()
{
    renderer.CreateInstance();
    renderer.createSurface(window);
    renderer.pickPhysicalDevice();
	renderer.createLogicalDevice();
    renderer.createSwapChain(window);
    renderer.createImageViews();
    renderer.createGraphicPipeline();
    renderer.createCommandPool();
    renderer.createCommandBuffers();
    renderer.createSyncObjects();
}

void Application::DestroyWindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::DestroyVulkan()
{
    
}
