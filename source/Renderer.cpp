#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Framebuffer.h"
#include <SOIL.h>
#include <gtc/matrix_transform.inl>


#define MV_MATRIX "uMV_Matrix"


int Renderer::width = 0;
int Renderer::height = 0;

Shader* Renderer::currentShader;
GameObject Renderer::scene;

Mesh* test2;
GameObject hat, bunny;
Camera* camera = new Camera();

GPUData Renderer::gpuData;

double lastTime;

Framebuffer* fboTest;
GLuint hatTex;

void Renderer::init(int window_width, int window_height) {
	width = window_width;
	height = window_height;

	gpuData.vaoHandle = -1;

	//Set Enables
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0, 0, .25, 1);

	currentShader = new Shader(
		"source/shaders/test.vs", "source/shaders/test.fs"
		);

	currentShader->use();

	hatTex = SOIL_load_OGL_texture
		(
			"assets/hat_tex.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
		);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hatTex);

	fboTest = new Framebuffer(512, 512, 2, false);


	scene.transform.translate(0, 0, -10);
	scene.transform.scale(2);

	scene.addChild(*camera);

    loadScene("assets/bunny.obj");
    bunny.addComponent(new Mesh("assets/bunny.obj"));
    bunny.transform.scale(2);

	scene.addChild(bunny);

	camera->transform.translate(0, 0, 10);

    loadScene("assets/hat.obj");
	hat.addComponent(new Mesh("assets/hat.obj"));
	hat.transform.translate(5, 3, -10);
	hat.transform.scale(5);

	Renderer::resize(width, height);

	lastTime = glfwGetTime();
}

void Renderer::loop() {
	glClearColor(0, 0, .25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	double dt = glfwGetTime() - lastTime;


	GLuint buffersToDraw[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	fboTest->bind(2, buffersToDraw);

	glClearColor(1, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, hatTex);
	scene.transform.rotate(glm::angleAxis(0.01f, glm::vec3(0, 1, 0)));
	bunny.transform.rotate(glm::angleAxis(0.01f, glm::vec3(0, 1, 0)));
    (*currentShader)[MV_MATRIX] = camera->getCameraMatrix() * bunny.transform.getTransformMatrix();
	bunny.getComponent<Mesh>()->draw();

	glBindTexture(GL_TEXTURE_2D, hatTex);

    (*currentShader)[MV_MATRIX] = camera->getCameraMatrix() * hat.transform.getTransformMatrix();
	hat.getComponent<Mesh>()->draw();

	fboTest->unbind();

	fboTest->blitFramebuffer(0, 0, 0, 512, 512);
	fboTest->blitFramebuffer(1, 10+512, 0, 512, 512);
}

Shader& Renderer::getCurrentShader() {
	return *currentShader;
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int window_width, int window_height)
{
	Renderer::resize(window_width, window_height);
}

void Renderer::resize(int width, int height) {
	glViewport(0, 0, width, height);

	glm::mat4 perspective = glm::perspective((float)(atan(1)*4.0f / 3.0f), width / (float)height, .1f, 100.f);
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
