#include "VE_window.hpp"
#include <stdexcept>
namespace VE {
	VEWindow::VEWindow(int _width, int _height, std::string _name) : width{ _width }, height{ _height }, name{_name} {
		initWindow();
	}
	VEWindow::~VEWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VEWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}


	void VEWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create windows surface!");
		}
	}


	void VEWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto veWindow = reinterpret_cast<VEWindow* >(glfwGetWindowUserPointer(window));
		veWindow->framebufferResized = true;
		veWindow->width = width;
		veWindow->height = height;
	
	}
}