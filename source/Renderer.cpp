#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Framebuffer.h"
#include <gtc/matrix_transform.inl>
#include "Skybox.h"
#include "TestSceneHawk.h"


#define MODEL_MATRIX "uM_Matrix"
#define VIEW_MATRIX "uV_Matrix"


int Renderer::width = 0;
int Renderer::height = 0;

Shader* Renderer::currentShader;
Shader* shaderList[SHADER_COUNT];

Camera* Renderer::camera = new Camera();

GPUData Renderer::gpuData;

std::list<RenderPass*> Renderer::passes;

double lastTime;

GLuint skyboxTex;

Framebuffer* fboTest;

glm::mat4 irradianceMatrix[3];

TestSceneHawk* testScene;


void Renderer::init(int window_width, int window_height) {
	width = window_width;
	height = window_height;

	gpuData.vaoHandle = -1;

	//Set Enables
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0, 0, .25, 1);
	glDepthFunc(GL_LEQUAL); //needed for skybox to overwrite blank z-buffer values

	shaderList[FORWARD_PBR_SHADER_ANIM] = new Shader(
		"source/shaders/forward_pbr_skeletal.vert", "source/shaders/forward_pbr.frag"
		);


	shaderList[FORWARD_PBR_SHADER] = new Shader(
		"source/shaders/forward_pbr.vert", "source/shaders/forward_pbr.frag"
		);

	(*shaderList[FORWARD_PBR_SHADER])["matTex"] = 2;

	shaderList[SKYBOX_SHADER] = new Shader(
		"source/shaders/skybox.vert", "source/shaders/skybox.frag"
		);

	shaderList[FBO_HDR] = new Shader(
		"source/shaders/fbo.vert", "source/shaders/fbo_hdr.frag"
		);

	currentShader = shaderList[FORWARD_PBR_SHADER];
	
	currentShader->use();


	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	std::string cubeFilenames[6] = {
		"assets/grace/grace_px.hdr",
		"assets/grace/grace_nx.hdr",
		"assets/grace/grace_py.hdr",
		"assets/grace/grace_ny.hdr",
		"assets/grace/grace_pz.hdr",
		"assets/grace/grace_nz.hdr" };

	skyboxTex = Skybox::loadCubemap(irradianceMatrix, cubeFilenames);

	(*shaderList[FORWARD_PBR_SHADER])["irradiance[0]"] = irradianceMatrix[0];
	(*shaderList[FORWARD_PBR_SHADER])["irradiance[1]"] = irradianceMatrix[1];
	(*shaderList[FORWARD_PBR_SHADER])["irradiance[2]"] = irradianceMatrix[2];
	(*shaderList[FORWARD_PBR_SHADER])["environment"] = 5;
	(*shaderList[FORWARD_PBR_SHADER])["environment_mipmap"] = 8.0f;


	(*shaderList[FORWARD_PBR_SHADER_ANIM])["irradiance[0]"] = irradianceMatrix[0];
	(*shaderList[FORWARD_PBR_SHADER_ANIM])["irradiance[1]"] = irradianceMatrix[1];
	(*shaderList[FORWARD_PBR_SHADER_ANIM])["irradiance[2]"] = irradianceMatrix[2];
	(*shaderList[FORWARD_PBR_SHADER_ANIM])["environment"] = 5;
	(*shaderList[FORWARD_PBR_SHADER_ANIM])["environment_mipmap"] = 8.0f;


	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	testScene = new TestSceneHawk();
	
	fboTest = new Framebuffer(width, height, 2, false, true);

	Renderer::resize(width, height);
    Renderer::passes.push_back(new DeferredPass(width, height));

	lastTime = glfwGetTime();
}


void Renderer::loop() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	double dt = glfwGetTime() - lastTime;

	GLuint buffersToDraw[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	fboTest->bind(2, buffersToDraw);

	testScene->loop();
		
	applyPerFrameData();
	GameObject::SceneRoot.draw();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
	Skybox::draw();
	
	fboTest->unbind();

	fboTest->bindTexture(0, 0);
	switchShader(FBO_HDR);
	fboTest->draw();
}

void Renderer::applyPerFrameData() {
	(*Renderer::getShader(FORWARD_PBR_SHADER))["cameraPos"] = Renderer::camera->transform.getWorldPosition();
	(*Renderer::getShader(FORWARD_PBR_SHADER_ANIM))["cameraPos"] = Renderer::camera->transform.getWorldPosition();
}



Shader& Renderer::getCurrentShader() {
	return *currentShader;
}

Shader* Renderer::getShader(int shaderId) {
	return shaderList[shaderId];
}

void Renderer::setCurrentShader(Shader* shader) {
	currentShader = shader;
}

void Renderer::switchShader(int shaderId) {
	currentShader = shaderList[shaderId];
	currentShader->use();
}

void Renderer::setModelMatrix(glm::mat4 transform) {
	(*currentShader)[MODEL_MATRIX] = transform;
	(*currentShader)[VIEW_MATRIX] = camera->getCameraMatrix();
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int window_width, int window_height)
{
	Renderer::resize(window_width, window_height);
}

void Renderer::resize(int width, int height) {
	glViewport(0, 0, width, height);

	glm::mat4 perspective = glm::perspective((float)(atan(1)*4.0f / 3.0f), width / (float)height, .1f, 100.f);
	for (Shader* shader : shaderList) {
		(*shader)["uP_Matrix"] = perspective;
	}
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
