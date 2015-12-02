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
int Renderer::shaderForwardLightList[2] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM };
int shaderViewList[4] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM, EMITTER_SHADER, EMITTER_BURST_SHADER };
int shaderCameraPosList[2] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM };
int shaderEnvironmentList[2] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM };
int shaderPerspectiveList[5] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM, SKYBOX_SHADER, EMITTER_SHADER, EMITTER_BURST_SHADER };

Camera* Renderer::camera = new Camera();

GPUData Renderer::gpuData;

std::list<RenderPass*> Renderer::passes;

double lastTime;

Framebuffer* fboTest;

TestSceneHawk* testScene;

Skybox* skybox;

ForwardPass *regularPass, *particlePass;

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

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	shaderList[FORWARD_PBR_SHADER_ANIM] = new Shader(
		"source/shaders/forward_pbr_skeletal.vert", "source/shaders/forward_pbr.frag"
		);


	shaderList[FORWARD_PBR_SHADER] = new Shader(
		"source/shaders/forward_pbr.vert", "source/shaders/forward_pbr.frag"
		);

	shaderList[SKYBOX_SHADER] = new Shader(
		"source/shaders/skybox.vert", "source/shaders/skybox.frag"
		);

	shaderList[FBO_HDR] = new Shader(
		"source/shaders/fbo.vert", "source/shaders/fbo_hdr.frag"
		);

	shaderList[EMITTER_SHADER] = new Shader(
		"source/shaders/gpu_particle.vert", "source/shaders/gpu_particle.frag"
		);

	shaderList[EMITTER_BURST_SHADER] = new Shader(
		"source/shaders/gpu_particle_burst.vert", "source/shaders/gpu_particle.frag"
		);

	currentShader = shaderList[FORWARD_PBR_SHADER];
	currentShader->use();

	std::string cubeFilenames[6] = {
		"assets/grace/grace_px.hdr",
		"assets/grace/grace_nx.hdr",
		"assets/grace/grace_py.hdr",
		"assets/grace/grace_ny.hdr",
		"assets/grace/grace_pz.hdr",
		"assets/grace/grace_nz.hdr" };
	skybox = new Skybox(cubeFilenames);
	skybox->applyIrradiance();
	skybox->applyTexture(5);

	testScene = new TestSceneHawk();
	
	fboTest = new Framebuffer(width, height, 2, false, true);

	Renderer::resize(width, height);

	regularPass = new ForwardPass();
	particlePass = new ForwardPass();

	Renderer::passes.push_back(regularPass);
   // Renderer::passes.push_back(new DeferredPass(width, height));
	Renderer::passes.push_back(particlePass);

	lastTime = glfwGetTime();
}


void Renderer::loop() {
    // START LOOP: NOTHING OUTSIDE OF THIS SHOULD BE HERE
    //             REFACTOR YOUR CODE!

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	applyPerFrameData();
	extractObjects();

	testScene->loop(); /* This is just temporary - all it does it do translation without having to create temporary components */
	skybox->draw();
    
	for(auto pass : passes)
    {
        pass->render();
    }

    // END LOOP
}

void Renderer::extractObjects() {
	GameObject::PassList passList;
	GameObject::SceneRoot.extract(passList);
	
	regularPass->setObjects(passList.forward);
	regularPass->setLights(passList.light);
	particlePass->setObjects(passList.particle);
}

void Renderer::applyPerFrameData() {
	for (int shaderId : shaderViewList) {
		(*Renderer::getShader(shaderId))[VIEW_MATRIX] = camera->getCameraMatrix();
	}
	for (int shaderId : shaderCameraPosList) {
		(*Renderer::getShader(shaderId))["cameraPos"] = Renderer::camera->transform.getWorldPosition();
	}
}

void Renderer::updatePerspective(const glm::mat4& perspectiveMatrix) {
	for (int shaderId : shaderPerspectiveList) {
		(*Renderer::getShader(shaderId))["uP_Matrix"] = perspectiveMatrix;
	}
}

void Renderer::setEnvironment(int slot, float mipmapLevels) {
	for (int shaderId : shaderEnvironmentList) {
		((*Renderer::getShader(shaderId)))["environment"] = slot;
		((*Renderer::getShader(shaderId)))["environment_mipmap"] = mipmapLevels;
	}
}

void Renderer::setIrradiance(glm::mat4 (&irradianceMatrix)[3]) {
	(*shaderList[FORWARD_PBR_SHADER])["irradiance[0]"] = irradianceMatrix[0];
	(*shaderList[FORWARD_PBR_SHADER])["irradiance[1]"] = irradianceMatrix[1];
	(*shaderList[FORWARD_PBR_SHADER])["irradiance[2]"] = irradianceMatrix[2];
	(*shaderList[FORWARD_PBR_SHADER_ANIM])["irradiance[0]"] = irradianceMatrix[0];
	(*shaderList[FORWARD_PBR_SHADER_ANIM])["irradiance[1]"] = irradianceMatrix[1];
	(*shaderList[FORWARD_PBR_SHADER_ANIM])["irradiance[2]"] = irradianceMatrix[2];
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
}

void Renderer::resize(int width, int height) {
	glViewport(0, 0, width, height);

	glm::mat4 perspective = glm::perspective((float)(atan(1)*4.0f / 3.0f), width / (float)height, .1f, 100.f);
	updatePerspective(perspective);
}