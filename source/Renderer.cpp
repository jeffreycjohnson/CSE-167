#include "Renderer.h"

#include "Mesh.h"
#include "Camera.h"
#include <memory>

Shader* Renderer::currentShader;
Mesh* meshComp;

Mesh* test2;
GameObject dragon;
Camera* camera = new Camera();

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

//	scene.addChild(BoxObject);
	meshComp = new Mesh("bunny.obj");
	scene.transform.translate(0, 0, -10);
	//scene.transform.scale(2);

	scene.addChild(*camera);

	GameObject* tmp = new GameObject();
	tmp->addComponent(meshComp);

	scene.addChild(*tmp);

	camera->transform.translate(0, 0, 10);

	test2 = new Mesh("hat.obj");
	dragon.addComponent(test2);
	dragon.transform.translate(5, 3, -10);
	dragon.transform.scale(2);


	Renderer::resize(width, height);

	lastTime = glfwGetTime();
}

void Renderer::loop() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float dt = glfwGetTime() - lastTime;
	scene.transform.rotate(glm::angleAxis(0.01f, glm::vec3(0, 1, 0)));
	meshComp->gameObject->transform.rotate(glm::angleAxis(0.01f, glm::vec3(0, 1, 0)));
	meshComp->draw(camera->getCameraMatrix());
	test2->draw(camera->getCameraMatrix());
	//camera.transform.translate(0.01, 0, 0);//.rotate(glm::angleAxis(.01f, glm::vec3(0, 1, 0)));
}

Shader& Renderer::getCurrentShader() {
	return *currentShader;
}



void Renderer::framebuffer_size_callback(GLFWwindow* window, int window_width, int window_height)
{
	Renderer::resize(window_width, window_height);
}

void Renderer::resize(int width, int height) {
	glm::mat4 perspective = glm::perspective((float)(atan(1)*4.0f / 3.0f), width / (float)height, .1f, 100.f);
	glViewport(0, 0, width, height);
	(Renderer::getCurrentShader())["uP_Matrix"] = perspective;
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
