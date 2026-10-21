#include "Surface.h"

void Surface::createSurface(GLFWwindow* window, Instance const& instance)
{
	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(*instance.GetInstance(), window, nullptr, &_surface) != 0)
	{
		throw std::runtime_error("failed to create window surface!");
	}

	surface = vk::raii::SurfaceKHR(instance.GetInstance(), _surface);
}
