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
#include "ObjectLoader.h"
#include "BoxCollider.h"
#include "Swarm.h"
#include "Animation.h"
#include "Light.h"
#include "ParticleTrail.h"
#include "Sound.h"
#include "PlayerController.h"
#include <gtx/rotate_vector.hpp>
#include "BoidAvoid.h"
#include "Turret.h"

GameObject *scene = new GameObject();
GameObject *camera;
GameObject *sun;
GameObject *bear;
GameObject *light;
GameObject *light2;
GameObject *sphere[8][8];
GameObject* emitter;
GPUEmitter* emitterComponent;
BoidSphere* ob1, *ob2;

Texture* testNormal;
Texture* bearTex, *bearSpec, *bearNormal;
Texture* blankNormal;

float tmp = 0;

GameObject* cruiser;


TestSceneHawk::TestSceneHawk()
{
	testNormal = new Texture("assets/test_sphere_normal.png", false);
	bearTex = new Texture("assets/bearTex2.jpg");
	bearSpec = new Texture("assets/bearTex2_spec.png", false);
	bearNormal = new Texture("assets/bearTex2_normal.jpg", false);
	Texture* cruiserTex = new Texture("assets/cruiser_tex.png");

	blankNormal = new Texture("assets/blank_normal.png", false);
	Texture* blueColor = new Texture("assets/blank_normal.png", true);


	addComponentMapping("boidAvoid", BoidAvoid::loadBoidAvoid);
	addComponentMapping("TurretPlaceholder", Turret::loadTurret);


	cruiser = loadScene("assets/cruiserPrototype.fbx");
	cruiser->transform.setPosition(-5, 0, 10);
	cruiser->transform.scale(0.25);
	cruiser->addComponent<BoidAvoid>(new BoidAvoid(1));
	GameObject::SceneRoot.addChild(*cruiser);

	for (int a = 0; a < 10; ++a) {
		GameObject* asteroid = loadScene("assets/asteroid3.fbx");
		//bad not psudo random location generation, just something quick to position asteroids
		asteroid->transform.translate(10 * sin(a * 21498), 10 * sin(a * 581209), 10 * sin(a * 12934));
		GameObject::SceneRoot.addChild(*asteroid);
	}

	Material* trailMaterial = new Material(Renderer::getShader(PARTICLE_TRAIL_SHADER));
	(*trailMaterial)["size"] = 0.32f;
	(*trailMaterial)["trailLength"] = 64.f; //set it to maxPoints
	(*trailMaterial)["colorTex"] = new Texture("assets/particle_trail.png");

	sun = loadScene("assets/test_sphere.obj");//new GameObject();
	auto sunLight = new DirectionalLight(true);
	sunLight->color = glm::vec3(0.5, 0.5, 0.5);
	sun->addComponent(sunLight);
	sun->transform.translate(5, 8.5, -1.5);
	sun->transform.setRotate(glm::quat_cast(glm::orientation(glm::vec3(0.5, 0.85, -0.15), glm::vec3(0, 1, 0))));
	Material* m = new Material(Renderer::getShader(FORWARD_PBR_SHADER));
	(*m)["useTextures"] = false;
	(*m)["testMetal"] = (0) / 7.f;
	(*m)["testRough"] = (0) / 7.f;
	(*m)["normalTex"] = blankNormal;
	sun->setMaterial(m);
	GameObject::SceneRoot.addChild(*sun);

	emitter = new GameObject();
	emitterComponent = new GPUEmitter(emitter, "assets/particles/particle.png", true);
	emitterComponent->init();
	emitter->addComponent(emitterComponent);
	emitter->transform.translate(0, 0, 2);
	GameObject::SceneRoot.addChild(*emitter);

	int numSquads = 40;
	int minCount = 3;
	int maxCount = 7;
	for (int i = 0; i < numSquads; i++)
	{
		int count = rand() % (maxCount - minCount) + minCount;
		GameObject** boids = new GameObject*[rand()];
		for (int i = 0; i < count; i++)
		{
			boids[i] = new GameObject();
			GameObject* boid = loadScene("assets/intercepter.dae");
			boids[i]->addChild(*boid);
			boids[i]->transform.scale(1);

			BoxCollider* collider = new BoxCollider(glm::vec3(0, 3, 2), glm::vec3(5, 7, 9));
			boids[i]->addComponent(collider);

			ParticleTrail* trail = new ParticleTrail();
			trail->material = trailMaterial;
			boids[i]->addComponent<ParticleTrail>(trail);

			GameObject::SceneRoot.addChild(*boids[i]);
		}
		GameObject* swarm = new GameObject();
		Swarm* swarmComponent = new Swarm(boids, count);
		swarm->addComponent(swarmComponent);
		GameObject::SceneRoot.addChild(*swarm);
	}
	/*ob1 = new BoidSphere();
	ob1->position = {-5, -6, 0};
	ob1->radius = 5;
	ob2 = new BoidSphere();
	ob2->position = { 6, 5, 0 };
	ob2->radius = 3;*/
//	Swarm::addObstacle(ob1);
//	Swarm::addObstacle(ob2);

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

	Sound* bearSound = new Sound("gun", false, false, 1);
	bear->addComponent(bearSound);

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

    GameObject::SceneRoot.addChild(*scene);
    camera = loadScene("assets/cockpit.fbx");
    scene->addChild(*camera);
    camera->addComponent(Renderer::camera);
	camera->transform.setPosition(0, 0, 20);
	camera->transform.getWorldPosition(); // Force matrix updates to prevent loud initial sounds
	Renderer::camera->getCameraMatrix();
	camera->update(0);

	Sound* camSound = new Sound("cabin", false, true, 1);
	camera->addComponent(camSound);

	PlayerController* controller = new PlayerController();
	camera->addComponent(controller);
}

void TestSceneHawk::loop() {

	tmp += 0.02f;
	light->transform.setPosition(5 * sin(tmp), 5 * cos(tmp), 4);

	light2->transform.setPosition(-5 * sin(tmp), -5 * cos(tmp), 4);

	cruiser->transform.rotate(glm::angleAxis(0.0075f, glm::vec3(1, 0, 0)));
	
	for (int x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			float zDist = -0.015 * cos(x + y + tmp);
			sphere[x][y]->transform.translate(0,0,zDist);
		}
	}

	BoxCollider::updateColliders();

	if (Input::getMouseDown("mouse 0"))
		Renderer::camera->screenShake(0.01, 0.25);
	if (Input::getKeyDown("space"))
		emitter->getComponent<GPUEmitter>()->play();
	if (Input::getMouseDown("mouse 0"))
		bear->getComponent<Sound>()->play();
	if (Input::getKeyDown("left control"))
		camera->getComponent<Sound>()->toggle();
}

void TestSceneHawk::debugDraw()
{
	GameObject::SceneRoot.debugDraw();
}

TestSceneHawk::~TestSceneHawk()
{
}
