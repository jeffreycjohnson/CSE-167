#ifndef INCLUDE_RENDERER_H
#define INCLUDE_RENDERER_H

#include "ForwardDecs.h"
#include "GameObject.h"
#include "Shader.h"

#include "glfw3.h"

class Renderer
{
	public:
		int width, height;

		GameObject scene;
		static Shader* currentShader;

		void init(int w, int h);
		void loop();
		static Shader& getCurrentShader();

		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void window_focus_callback(GLFWwindow* window, int focused);

		//glfwSetScrollCallback(window, scroll_callback);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif