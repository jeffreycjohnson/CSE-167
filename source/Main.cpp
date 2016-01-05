#include "ForwardDecs.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Input.h"
#include "Timer.h"
#include "Sound.h"
#include <glfw3.h>
#include "ObjectLoader.h"
#include <iostream>

int main()
{
	Timer::init(2.5f);
    if (!glfwInit())
    {
        LOG("GLFW failed to initialize");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	int width = 1280;
	int height = 1024;
	
	//zconst GLFWvidmode* mode = glfwGetVideoMode(monitor);
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
	Sound::init();
	Renderer::init(width, height);
	Input::init(window);
	Input::setCursor("assets/cursor/cursor.png", 32, 32);
	//mouse events
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetScrollCallback(window, Input::scroll_callback);

    // Loads mesh data for primatives, but we don't need it in a GameObject
    delete loadScene("assets/Primatives.obj");

	while (!glfwWindowShouldClose(window))
	{
        GameObject::SceneRoot.update(Timer::deltaTime());
		Input::update();
		Sound::updateFMOD();
		Renderer::loop();
		Timer::update();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}
