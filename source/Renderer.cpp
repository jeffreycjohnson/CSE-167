#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Framebuffer.h"
#include <SOIL.h>
#include <gtc/matrix_transform.inl>
#include "Skybox.h"


#define MODEL_MATRIX "uM_Matrix"
#define VIEW_MATRIX "uV_Matrix"


int Renderer::width = 0;
int Renderer::height = 0;

Shader* Renderer::currentShader;
#define SHADER_COUNT 3
#define REGULAR_SHADER 0
#define FORWARD_PBR_SHADER 1
#define SKYBOX_SHADER 2
Shader* shaderList[SHADER_COUNT];

GameObject Renderer::scene;

Mesh* test2;
GameObject *hat, *bunny, *bagel, *barrel;
GameObject* turret;
GameObject *tankTop, *tankBot;
Camera* camera = new Camera();

GPUData Renderer::gpuData;

double lastTime;

Framebuffer* fboTest;
GLuint hatTex, turretTex, bagelTex, skyboxTex;
GLuint barrelTex, barrelTex_Mat;
GLuint tankTexTop, tankTexBot, tankSpecTop, tankSpecBot;

float tmp = 0;

glm::mat4 irradianceMatrix[3];


void Renderer::init(int window_width, int window_height) {
	width = window_width;
	height = window_height;

	gpuData.vaoHandle = -1;

	//Set Enables
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0, 0, .25, 1);

	shaderList[REGULAR_SHADER] = new Shader(
		"source/shaders/forward_pbr.vert", "source/shaders/forward_pbr.frag"
		);


	shaderList[FORWARD_PBR_SHADER] = new Shader(
		"source/shaders/forward_pbr.vert", "source/shaders/forward_pbr.frag"
		);

	(*shaderList[FORWARD_PBR_SHADER])["matTex"] = 2;

	shaderList[SKYBOX_SHADER] = new Shader(
		"source/shaders/skybox.vert", "source/shaders/skybox.frag"
		);

	currentShader = shaderList[REGULAR_SHADER];
	
	currentShader->use();

	hatTex = SOIL_load_OGL_texture
		(
			"assets/hat_tex.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
		);
	turretTex = SOIL_load_OGL_texture
		(
			"assets/turret_tex.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
			);
	bagelTex = SOIL_load_OGL_texture
		(
			"assets/bagel_everything_tex.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
			);


	barrelTex = SOIL_load_OGL_texture
		(
			"assets/WoodenBarrel/barrel_tex.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
			);

	barrelTex_Mat = SOIL_load_OGL_texture
		(
			"assets/WoodenBarrel/barrel_spec.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
			);



	tankTexTop = SOIL_load_OGL_texture
		(
			"assets/hover_tank/hoverTank1_top_tex.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
			);
	tankTexBot = SOIL_load_OGL_texture
		(
			"assets/hover_tank/hoverTank1_bot_tex.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
			);
	tankSpecTop = SOIL_load_OGL_texture
		(
			"assets/hover_tank/hoverTank1_top_spec.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
			);
	tankSpecBot = SOIL_load_OGL_texture
		(
			"assets/hover_tank/hoverTank1_bot_spec.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y
			);


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


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hatTex);

	fboTest = new Framebuffer(1800, 1800, 2, false);


	scene.transform.translate(0, 0, 0);
	//scene.transform.scale(2);

	scene.addChild(*camera);

    bunny = loadScene("assets/bunny.obj");
    bunny->transform.scale(2);
	bunny->transform.translate(-2, 0, -10);
	//scene.addChild(*bunny);

	camera->transform.translate(0, 0, 20);
	//camera->transform.rotate(glm::angleAxis(atanf(1), glm::vec3(-1, 0, 0)));

    turret = loadScene("assets/turret.dae");
	turret->transform.translate(-1, -2, 5);
	//turret->transform.rotate(glm::angleAxis(atanf(1)/3, glm::vec3(0, 1, 0)));

	hat = loadScene("assets/hat.obj");
	hat->transform.translate(0,1,0);
	hat->transform.scale(5);
	bunny->addChild(*hat);

	bagel = loadScene("assets/bagel.obj");
	bagel->transform.scale(20);
	//bagel->transform.translate(-5, 7, -10);
	//scene.addChild(*bagel);

	barrel = loadScene("assets/sphere.obj");
	barrel->transform.scale(1);

	tankBot = loadScene("assets/hover_tank/hoverTank1_bot.obj");
	tankTop = loadScene("assets/hover_tank/hoverTank1_top.obj");

	Renderer::resize(width, height);

	lastTime = glfwGetTime();
}

void Renderer::loop() {
	glClearColor(0, 0, .25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	double dt = glfwGetTime() - lastTime;

	currentShader = shaderList[FORWARD_PBR_SHADER];
	currentShader->use();
	(*currentShader)["cameraPos"] = camera->transform.getWorldPosition();

	GLuint buffersToDraw[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	fboTest->bind(2, buffersToDraw);

	glClearColor(1, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.transform.rotate(glm::angleAxis(0.003f, glm::vec3(0, 1, 0)));
	//camera->transform.rotate(glm::angleAxis(-0.004f, glm::vec3(-1, 0, 0)));

	//TODO REMOVE
	currentShader = shaderList[FORWARD_PBR_SHADER];
	currentShader->use();

	for (int x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			float xDist = 2 * (x - 3.5);
			float yDist = 2 * (y - 3.5);
			barrel->transform.translate(xDist, yDist, 0);
			(*shaderList[FORWARD_PBR_SHADER])["testMetal"] = x/7.0f;
			(*shaderList[FORWARD_PBR_SHADER])["testRough"] = y/7.0f;
			barrel->draw();
			barrel->transform.translate(-xDist, -yDist, 0);
		}
	}


	/*
	glBindTexture(GL_TEXTURE_2D, hatTex);
	bunny->draw();

	glBindTexture(GL_TEXTURE_2D, hatTex);
	hat->draw();


	glBindTexture(GL_TEXTURE_2D, bagelTex);
	bagel->draw();
	*/

	/*
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, tankSpecBot);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tankTexBot);
	tankBot->draw();

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, tankSpecTop);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tankTexTop);
	tankTop->draw();



	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, turretTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, turretTex);
	turret->draw();
	*/


	(*shaderList[FORWARD_PBR_SHADER])["uLightData[0]"] = glm::vec4(5*sin(tmp), 0, 4, 1);
	(*shaderList[FORWARD_PBR_SHADER])["uLightData[1]"] = glm::vec4(1, 1, 1, 10);
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);


	currentShader = shaderList[SKYBOX_SHADER];
	currentShader->use();

	glDepthFunc(GL_LEQUAL); //needed for skybox to overwrite blank z-buffer values
	(*currentShader)[VIEW_MATRIX] = camera->getCameraMatrix();
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
	Skybox::draw();

	currentShader = shaderList[REGULAR_SHADER];
	currentShader->use();


	turret->transform.translate(0.2f*sin(tmp+=0.05f), 0, 0);

	////Turret targeting code
	glm::vec3 target = bagel->transform.getWorldPosition();


	glm::vec3 turretNormal = glm::normalize(glm::vec3(turret->transform.getTransformMatrix() * glm::vec4(0, 0, 1, 0)));
	glm::vec3 turretX = glm::normalize(glm::vec3(turret->transform.getTransformMatrix() * glm::vec4(1, 0, 0, 0)));
	glm::vec3 turretZ = glm::cross(turretX, turretNormal);

	glm::mat3 turretBasis(turretX, turretNormal, turretZ);


	glm::vec3 turretPos(turret->transform.getWorldPosition());
	glm::vec3 diff;
	diff.x = target.x - turretPos.x;
	diff.y = target.y - turretPos.y;
	diff.z = target.z - turretPos.z;

	diff = turretBasis * diff;
	float yaw = atan2f(diff.x, diff.z);
	float pitch = -atan2f(diff.y, sqrtf(diff.x*diff.x + diff.z*diff.z));

	turret->transform.children[0]->children[0]->setRotate(glm::angleAxis(yaw, glm::vec3(0, 0, 1)));
	turret->transform.children[0]->children[0]->children[0]->setRotate(glm::angleAxis(pitch, glm::vec3(1, 0, 0)));
	////-----------

	fboTest->unbind();

	fboTest->blitFramebuffer(0, 0, 0, 900, 900);
	fboTest->blitFramebuffer(1, 10+900, 0, 900, 900);
}

Shader& Renderer::getCurrentShader() {
	return *currentShader;
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
