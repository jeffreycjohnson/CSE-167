#include "TestSceneHawk.h"
#include <SOIL.h>
#include "GameObject.h"
#include "Mesh.h"
#include "Camera.h"
#include "GPUEmitter.h"
#include "Input.h"
#include "Timer.h"
#include "Skybox.h"
#include "Renderer.h"
#include "Texture.h"

#include "Timer.h"
#include "ObjectLoader.h"
#include "GPUEmitter.h"
#include "Swarm.h"
#include "Input.h"
#include "Animation.h"
#include "Light.h"

GameObject* scene = new GameObject();
GameObject *bear;
GameObject *light;
GameObject *light2;
GameObject *sphere[8][8];
GameObject* emitter;
GPUEmitter* emitterComponent;
GameObject* boid;
Swarm* swarm;
BoidSphere* ob1, *ob2;

Texture* testNormal;
Texture* bearTex, *bearSpec, *bearNormal;
Texture* blankNormal;

float tmp = 0;


TestSceneHawk::TestSceneHawk()
{
	testNormal = new Texture("assets/test_sphere_normal.png");
	bearTex = new Texture("assets/bearTex2.jpg");
    bearSpec = new Texture("assets/bearTex2_spec.png");
    bearNormal = new Texture("assets/bearTex2_normal.jpg");

	blankNormal = new Texture("assets/blank_normal.png");

	GameObject::SceneRoot.addChild(*scene);
	scene->addChild(*Renderer::camera);
	Renderer::camera->transform.translate(0, 0, 20);

	emitter = new GameObject();
	emitterComponent = new GPUEmitter(emitter, "assets/particles/particle.png", true);
	emitterComponent->init();
	emitter->addComponent(emitterComponent);
	emitter->transform.translate(0, 0, 2);
	GameObject::SceneRoot.addChild(*emitter);

	boid = loadScene("assets/test_sphere.obj");
	swarm = new Swarm(boid, 50);
	ob1 = new BoidSphere();
	ob1->position = {-5, -6, 0};
	ob1->radius = 5;
	Swarm::addObstacle(ob1);
	ob2 = new BoidSphere();
	ob2->position = { 6, 5, 0 };
	ob2->radius = 3;
	Swarm::addObstacle(ob2);

	bear = loadScene("assets/bear2.dae");
	bear->transform.rotate(glm::angleAxis(atanf(1)*1.f, glm::vec3(1, 0, 0)));
	bear->transform.translate(-5, 0, 1);

	Material* bearMat = new Material(Renderer::getShader(DEFERRED_PBR_SHADER_ANIM), false);
	(*bearMat)["useTextures"] = true;
	(*bearMat)["colorTex"] = bearTex;
	(*bearMat)["matTex"] = bearSpec;
	(*bearMat)["normalTex"] = bearNormal;
	bear->setMaterial(bearMat);
	bear->getComponent<Animation>()->play(0, true);

	GameObject::SceneRoot.addChild(*bear);

	for (int x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			sphere[x][y] = loadScene("assets/test_sphere.obj");
			
			float xDist = 2 * (x - 3.5);
			float yDist = 2 * (y - 3.5);
			float zDist = sin(x + y);
			sphere[x][y]->transform.translate(xDist, yDist, -3 + zDist);

			Material* sphereMat = new Material(Renderer::getShader(FORWARD_PBR_SHADER));
			(*sphereMat)["useTextures"] = false;
			(*sphereMat)["testMetal"] = (x) / 7.f;
			(*sphereMat)["testRough"] = (y) / 7.f;
			(*sphereMat)["normalTex"] = testNormal;
			sphere[x][y]->setMaterial(sphereMat);

			GameObject::SceneRoot.addChild(*sphere[x][y]);
		}
	}

	light = loadScene("assets/test_sphere.obj");
	Material* sphereMat = new Material(Renderer::getShader(FORWARD_PBR_SHADER));
	(*sphereMat)["useTextures"] = false;
	(*sphereMat)["testMetal"] = (0) / 7.f;
	(*sphereMat)["testRough"] = (0) / 7.f;
	(*sphereMat)["normalTex"] = blankNormal;
	Light* lightComponent = new PointLight();
	lightComponent->color = glm::vec3(1, 1, 1);
	light->addComponent<Light>(lightComponent);
	light->setMaterial(sphereMat);

	GameObject::SceneRoot.addChild(*light);

	light2 = loadScene("assets/test_sphere.obj");
	sphereMat = new Material(Renderer::getShader(FORWARD_PBR_SHADER));
	(*sphereMat)["useTextures"] = false;
	(*sphereMat)["testMetal"] = (0) / 7.f;
	(*sphereMat)["testRough"] = (0) / 7.f;
	(*sphereMat)["normalTex"] = blankNormal;
	Light* light2Component = new PointLight();
	light2Component->color = glm::vec3(10, 10, 10);
	light2->addComponent<Light>(light2Component);
	light2->setMaterial(sphereMat);

	GameObject::SceneRoot.addChild(*light2);
}

void TestSceneHawk::loop() {
	scene->transform.rotate(glm::angleAxis(0.001f, glm::vec3(0, 1, 0)));

	tmp += 0.02f;
	light->transform.setPosition(5 * sin(tmp), 5 * cos(tmp), 4);

	light2->transform.setPosition(-5 * sin(tmp), -5 * cos(tmp), 4);
	
	for (int x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			float zDist = -0.015 * cos(x + y + tmp);
			sphere[x][y]->transform.translate(0,0,zDist);
		}
	}

	if (Input::getKeyDown("space"))
		emitter->getComponent<GPUEmitter>()->play();

	emitter->update(Timer::time());
	emitter->draw();

	swarm->update(Timer::time());
	swarm->draw();

	bear->getComponent<Animation>()->update(Timer::deltaTime());
}

TestSceneHawk::~TestSceneHawk()
{
}
