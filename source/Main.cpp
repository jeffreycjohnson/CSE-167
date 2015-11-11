#include "ForwardDecs.h"
#include "glfw3.h"
#include "gtc/matrix_inverse.hpp""
#include <iostream>
#include <gtx/string_cast.hpp>
#include "Renderer.h"

int main()
{
    if (!glfwInit())
    {
        LOG("GLFW failed to initialize");
        return EXIT_FAILURE;
    }

    // Use OpenGL 4.5 for Direct State Access
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
   // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	int width = 1600;
	int height = 900;

	//const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(width, height, "CSE 167 Final Project", nullptr, nullptr);

	//set callbacks
	//glfwSetWindowFocusCallback(window, window_focus_callback);
	//void window_focus_callback(GLFWwindow* window, int focused)

    if (!window)
    {
        LOG("Window creation failed");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();
	glfwSwapInterval(1);
	Renderer renderer;
	renderer.init(width, height);

	//window events
	glfwSetFramebufferSizeCallback(window, Renderer::framebuffer_size_callback);
	glfwSetWindowFocusCallback(window, Renderer::window_focus_callback);

	glfwSetKeyCallback(window, Renderer::key_callback);

	//mouse events
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, Renderer::cursor_position_callback);
	glfwSetMouseButtonCallback(window, Renderer::mouse_button_callback); //note - if presses aren't working, try sticky mouse mode
	glfwSetScrollCallback(window, Renderer::scroll_callback);

	while (!glfwWindowShouldClose(window))
	{
		renderer.loop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}