#include "Renderer.h"

#include "Mesh.h"


Shader* Renderer::currentShader;
Mesh* meshComp;

double lastTime;

void Renderer::init(int window_width, int window_height) {
	width = window_width;
	height = window_height;

	//Set Enables
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0, 0, .25, 1);

	currentShader = new Shader(
		"source/shaders/test.vs", "source/shaders/test.fs"
		);

	currentShader->use();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(glGetAttribLocation(currentShader->id, "aPosition"));
	glEnableVertexAttribArray(glGetAttribLocation(currentShader->id, "aNormal"));

//	scene.addChild(BoxObject);
	meshComp = new Mesh("bunny.obj");
	scene.addComponent(meshComp);
	scene.transform.translate(0, 0, -10);
	scene.transform.scale(2);

	glm::mat4 perspective = glm::perspective((float)(atan(1)*4.0f/3.0f), width/(float)height, .1f, 100.f);
	glViewport(0, 0, 1600, 800);
	(*currentShader)["uP_Matrix"] = perspective;

	lastTime = glfwGetTime();
}

void Renderer::loop() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float dt = glfwGetTime() - lastTime;
	scene.transform.rotate(glm::angleAxis(0.01f, glm::vec3(0, 1, 0)));
	meshComp->draw();
}

Shader& Renderer::getCurrentShader() {
	return *currentShader;
}



void Renderer::framebuffer_size_callback(GLFWwindow* window, int window_width, int window_height)
{
/*	width = window_width;
	height = window_height;
	glViewport(0, 0, width, height);*/
}

void Renderer::window_focus_callback(GLFWwindow* window, int focused)
{
	if (focused)
	{
		// The window gained input focus
	}
	else
	{
		// The window lost input focus
	}
}

void Renderer::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
}

void Renderer::cursor_position_callback(GLFWwindow * window, double xpos, double ypos)
{
}

void Renderer::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
}

void Renderer::scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
}
