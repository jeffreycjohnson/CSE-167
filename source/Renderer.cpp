#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Framebuffer.h"
#include <gtc/matrix_transform.hpp>
#include "Skybox.h"
#include "Timer.h"
#include "Light.h"
#include "GameScene.h"
#include "ThreadPool.h"
#include "RenderPass.h"


#define MODEL_MATRIX "uM_Matrix"
#define VIEW_MATRIX "uV_Matrix"


int Renderer::windowWidth = 0;
int Renderer::windowHeight = 0;

glm::mat4 Renderer::view, Renderer::perspective;

Shader* Renderer::currentShader;
Shader* shaderList[SHADER_COUNT];
int Renderer::shaderForwardLightList[] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM };
int shaderViewList[] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM, EMITTER_SHADER, EMITTER_BURST_SHADER,
    PARTICLE_TRAIL_SHADER, DEFERRED_PBR_SHADER, DEFERRED_PBR_SHADER_ANIM, DEFERRED_SHADER_LIGHTING, SKYBOX_SHADER,
    SHADOW_SHADER, SHADOW_SHADER_ANIM, BASIC_SHADER, FORWARD_UNLIT, FORWARD_EMISSIVE };
int shaderCameraPosList[] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM, DEFERRED_SHADER_LIGHTING };
int shaderEnvironmentList[] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM, DEFERRED_SHADER_LIGHTING };
int shaderPerspectiveList[] = { FORWARD_PBR_SHADER, FORWARD_PBR_SHADER_ANIM, SKYBOX_SHADER, EMITTER_SHADER,
    EMITTER_BURST_SHADER, PARTICLE_TRAIL_SHADER, DEFERRED_PBR_SHADER, DEFERRED_PBR_SHADER_ANIM, DEFERRED_SHADER_LIGHTING,
    BASIC_SHADER, FORWARD_UNLIT, FORWARD_EMISSIVE };

Camera* Renderer::mainCamera;
Camera* Renderer::currentCamera;
std::list<Camera*> Renderer::cameras;

GPUData Renderer::gpuData;

Renderer::RenderBuffer Renderer::renderBuffer;

Scene* scene;

Skybox* skybox;

void Renderer::init(int window_width, int window_height) {
    windowWidth = window_width;
    windowHeight = window_height;

	gpuData.vaoHandle = -1;

	//Set Enables
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0, 0, 0, 1);
	glDepthFunc(GL_LEQUAL); //needed for skybox to overwrite blank z-buffer values

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	shaderList[FORWARD_PBR_SHADER_ANIM] = new Shader(
		"source/shaders/forward_pbr_skeletal.vert", "source/shaders/forward_pbr.frag"
		);


	shaderList[FORWARD_PBR_SHADER] = new Shader(
		"source/shaders/forward_pbr.vert", "source/shaders/forward_pbr.frag"
		);

    shaderList[DEFERRED_PBR_SHADER_ANIM] = new Shader(
        "source/shaders/forward_pbr_skeletal.vert", "source/shaders/deferred_gbuffer.frag"
        );


    shaderList[DEFERRED_PBR_SHADER] = new Shader(
        "source/shaders/forward_pbr.vert", "source/shaders/deferred_gbuffer.frag"
        );

    shaderList[DEFERRED_SHADER_LIGHTING] = new Shader(
        "source/shaders/deferred_lighting.vert", "source/shaders/deferred_lighting.frag"
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

	shaderList[PARTICLE_TRAIL_SHADER] = new Shader(
		"source/shaders/particle_trail.vert", "source/shaders/particle_trail.frag"
		);

    shaderList[SHADOW_SHADER] = new Shader(
        "source/shaders/forward_pbr.vert", "source/shaders/shadow.frag"
        );

    shaderList[SHADOW_SHADER_ANIM] = new Shader(
        "source/shaders/forward_pbr_skeletal.vert", "source/shaders/shadow.frag"
        );

    (*shaderList[SHADOW_SHADER])["uP_Matrix"] = DirectionalLight::shadowMatrix;
    (*shaderList[SHADOW_SHADER_ANIM])["uP_Matrix"] = DirectionalLight::shadowMatrix;

	shaderList[BASIC_SHADER] = new Shader(
		"source/shaders/simple.vert", "source/shaders/simple.frag"
		);

    shaderList[FORWARD_UNLIT] = new Shader(
        "source/shaders/forward_pbr.vert", "source/shaders/forward_unlit.frag"
        );
    shaderList[FORWARD_EMISSIVE] = new Shader(
        "source/shaders/forward_pbr.vert", "source/shaders/emissive.frag"
        );

	shaderList[FBO_BLUR] = new Shader(
		"source/shaders/fbo.vert", "source/shaders/fbo_blur.frag"
		);

	shaderList[FBO_PASS] = new Shader(
		"source/shaders/fbo.vert", "source/shaders/fbo_pass.frag"
		);

	currentShader = shaderList[FORWARD_PBR_SHADER];
	currentShader->use();

	std::string cubeFilenames[6] = {
		"assets/skybox/right.hdr",
		"assets/skybox/left.hdr",
		"assets/skybox/top.hdr",
		"assets/skybox/bottom.hdr",
		"assets/skybox/front.hdr",
		"assets/skybox/back.hdr"};
	skybox = new Skybox(cubeFilenames);
	skybox->applyIrradiance();
	skybox->applyTexture(5);

    mainCamera = new Camera(windowWidth, windowHeight, false);
    mainCamera->passes.push_back(std::make_unique<ShadowPass>());
    mainCamera->passes.push_back(std::make_unique<DeferredPass>());
    mainCamera->passes.push_back(std::make_unique<SkyboxPass>(skybox));
    mainCamera->passes.push_back(std::make_unique<ForwardPass>());
    mainCamera->passes.push_back(std::make_unique<ParticlePass>());
    mainCamera->passes.push_back(std::make_unique<BloomPass>());

    resize(windowWidth, windowHeight);

	scene = new GameScene();
}

void Renderer::loop() {
    extractObjects();

    for(auto camera : cameras)
    {
        camera->update(Timer::deltaTime());
        if (camera == mainCamera) continue;
        currentCamera = camera;
        applyPerFrameData(camera);
        for(auto& pass : camera->passes)
        {
            pass->render(camera);
        }
    }
    currentCamera = mainCamera;
    applyPerFrameData(mainCamera);
    int i = 0;
    for (auto& pass : mainCamera->passes)
    {
        if (i++ == mainCamera->passes.size() - 1) {
            auto job = workerPool->createJob(GameObject::UpdateScene)->queue();
            pass->render(mainCamera);
            workerPool->wait(job);
        }
        else pass->render(mainCamera);
    }

    (*shaderList[SHADOW_SHADER])["uP_Matrix"] = DirectionalLight::shadowMatrix;
    (*shaderList[SHADOW_SHADER_ANIM])["uP_Matrix"] = DirectionalLight::shadowMatrix;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::extractObjects() {
    renderBuffer.deferred.clear();
    renderBuffer.forward.clear();
    renderBuffer.particle.clear();
    renderBuffer.light.clear();
	GameObject::SceneRoot.extract();
}

void Renderer::applyPerFrameData(Camera* camera) {
    view = camera->getCameraMatrix();
	for (int shaderId : shaderViewList) {
		(*Renderer::getShader(shaderId))[VIEW_MATRIX] = view;
	}
	for (int shaderId : shaderCameraPosList) {
		(*Renderer::getShader(shaderId))["cameraPos"] = camera->gameObject->transform.getWorldPosition();
	}
}

void Renderer::updatePerspective(const glm::mat4& perspectiveMatrix) {
    perspective = perspectiveMatrix;
	for (int shaderId : shaderPerspectiveList) {
		(*Renderer::getShader(shaderId))["uP_Matrix"] = perspective;
	}
}

void Renderer::setEnvironment(int slot, float mipmapLevels) {
	for (int shaderId : shaderEnvironmentList) {
		((*Renderer::getShader(shaderId)))["environment"] = slot;
		((*Renderer::getShader(shaderId)))["environment_mipmap"] = mipmapLevels;
	}
}

void Renderer::setIrradiance(glm::mat4 (&irradianceMatrix)[3]) {
    for (int shaderId : shaderEnvironmentList) {
        ((*Renderer::getShader(shaderId)))["irradiance[0]"] = irradianceMatrix[0];
        ((*Renderer::getShader(shaderId)))["irradiance[1]"] = irradianceMatrix[1];
        ((*Renderer::getShader(shaderId)))["irradiance[2]"] = irradianceMatrix[2];
    }
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

void Renderer::setModelMatrix(const glm::mat4& transform) {
	(*currentShader)[MODEL_MATRIX] = transform;
}

void Renderer::resize(int width, int height) {
	glViewport(0, 0, width, height);

	windowWidth = width;
	windowHeight = height;

	perspective = glm::perspective(mainCamera->getFOV(), width / (float)height, NEAR_DEPTH, FAR_DEPTH);
	updatePerspective(perspective);
}