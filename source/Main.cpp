#include "ForwardDecs.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Material.h"
#include "Input.h"
#include "Timer.h"
#include <glfw3.h>
#include <iostream>

class FrameTimer
{
public:
    explicit FrameTimer(float delta)
        : fps(0), deltaTime(0.0f), oldTime(0.0f), prevTime(0.0f), delta(delta), frames(0)
    {
        oldTime = static_cast<float>(glfwGetTime());
    }

    void tick()
    {
        float time = static_cast<float>(glfwGetTime());
        deltaTime = time - prevTime;
        prevTime = time;
        frames++;
        if (time - oldTime > delta)
        {
            std::cout << (time - oldTime) / frames * 1000 << " ms (";
            std::cout << frames / (time - oldTime) << " fps)" << std::endl;
            fps = static_cast<int>(frames / (time - oldTime) + 0.5f);
            oldTime = time;
            frames = 0;
        }
    }

    int fps;
    float deltaTime;

private:
    float oldTime, prevTime, delta, frames;
};

extern GameObject scene;

int main()
{
    FrameTimer timer(2.5f);
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
	Renderer::init(width, height);
	Input::init(window);
	Timer::init();
    //Material m(new Shader("", ""));
    //m["test"] = 2.0f;
	//window events
	glfwSetFramebufferSizeCallback(window, Renderer::framebuffer_size_callback);
	glfwSetWindowFocusCallback(window, Renderer::window_focus_callback);

	glfwSetKeyCallback(window, Renderer::key_callback);

	//mouse events
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, Renderer::cursor_position_callback);
	glfwSetMouseButtonCallback(window, Renderer::mouse_button_callback); //note - if presses aren't working, try sticky mouse mode
	glfwSetScrollCallback(window, Input::scroll_callback);

	while (!glfwWindowShouldClose(window))
	{
        timer.tick();
        scene.update(timer.deltaTime);
		Input::update();
		Timer::update();
		Renderer::loop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}
