#include "ForwardDecs.h"
#include "glfw3.h"

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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(1600, 800, "CSE 167 Final Project", nullptr, nullptr);
    if (!window)
    {
        LOG("Window creation failed");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();
}
