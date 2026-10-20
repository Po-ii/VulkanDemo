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
        renderer.drawFrame();
    }
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, "VulkanDemo", nullptr, nullptr);
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
    renderer.cleanupSwapChain();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::DestroyVulkan()
{
}
