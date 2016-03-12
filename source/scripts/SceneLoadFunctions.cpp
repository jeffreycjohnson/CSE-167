#include "SceneLoadFunctions.h"
#include "Mesh.h"
#include "ObjectLoader.h"
#include "BoidAvoid.h"
#include "Renderer.h"
#include "Swarm.h"
#include "Camera.h"
#include "Material.h"
#include "ParticleTrail.h"
#include "PlayerController.h"
#include "Sound.h"
#include "BoxCollider.h"
#include "Fighter.h"

void loadX(GameObject* parent, std::string filename) {
	parent->removeComponent<Mesh>();

	GameObject* childObj = loadScene(filename);
	childObj->transform.rotate(glm::angleAxis(atanf(1)*2.f, glm::vec3(1, 0, 0))); //convert back to z-up axis
	parent->addChild(childObj);
}

void SceneLoadFunctions::loadPlayer(GameObject* parent) {

	GameObject* camera = loadScene("assets/cockpit.fbx");
	parent->addChild(camera);
	camera->addComponent(Renderer::mainCamera);
	camera->transform.setPosition(0, 0, 20);
	camera->transform.getWorldPosition(); // Force matrix updates to prevent loud initial sounds
	Renderer::mainCamera->getCameraMatrix();
	camera->update(0);

	Sound* camSound = new Sound("cabin", true, true, 0.5f, false);
	camera->addComponent(camSound);

	GameObject* gunObj = new GameObject();
	Sound* gun = new Sound("gun", false, false, 0.5f, false);
	gunObj->addComponent(gun);
	camera->addChild(gunObj);

	GameObject* boostObj = new GameObject();
	Sound* boost = new Sound("boost", true, true, 0.0f, false);
	boostObj->addComponent(boost);
	camera->addChild(boostObj);

	PlayerController* controller = new PlayerController(gun, boost);
	camera->addComponent(controller);

	GameObject* musicObj = new GameObject();
	Sound* music = new Sound("music", true, true, 0.5f, false);
	musicObj->addComponent(music);
	parent->addChild(musicObj);

	BoidAvoid* obstacle = new BoidAvoid(5);
	parent->addComponent(obstacle);
}

void SceneLoadFunctions::loadAsteroidBoid(GameObject* parent) {
	loadX(parent, "assets/asteroid3.fbx");
	parent->addComponent<BoidAvoid>(new BoidAvoid(25));
}

void SceneLoadFunctions::loadAsteroidNoBoid(GameObject* parent) {
	return loadX(parent, "assets/asteroid3.fbx");
}

void SceneLoadFunctions::loadCapitalShip(GameObject* parent) {
	parent->removeComponent<Mesh>();

	GameObject* childObj = loadScene("assets/cruiserPrototype.fbx");
	childObj->transform.rotate(glm::angleAxis(atanf(1)*2.f, glm::vec3(1, 0, 0))); //convert back to z-up axis

	loadX(parent, "assets/cruiser_beam.fbx");

	parent->addChild(childObj);
}

void loadWing(GameObject* parent, bool allied) {
	parent->removeComponent<Mesh>();

	Material* trailMaterial = new Material(Renderer::getShader(PARTICLE_TRAIL_SHADER));
	(*trailMaterial)["size"] = 0.32f;
	(*trailMaterial)["trailLength"] = 64.f; //set it to maxPoints
	(*trailMaterial)["colorTex"] = new Texture("assets/particle_trail.png");

	int minCount = 3;
	int maxCount = 7;

	int count = rand() % (maxCount - minCount) + minCount;
	GameObject** boids = new GameObject*[count];
	for (int i = 0; i < count; i++)
	{
		boids[i] = new GameObject();
		GameObject* boid = loadScene("assets/intercepter.dae");
		boids[i]->addChild(boid);
		boids[i]->transform.scale(1);
		boids[i]->transform.translate(parent->transform.getWorldPosition());

		BoxCollider* collider = new BoxCollider(glm::vec3(0, 0, 2), glm::vec3(6, 3, 10));
		boids[i]->addComponent(collider);

		ParticleTrail* trail = new ParticleTrail();
		trail->material = trailMaterial;
		boids[i]->addComponent<ParticleTrail>(trail);

		Sound* explosion = new Sound("explosion", false, false, 20, true);
		boids[i]->addComponent<Sound>(explosion);

		Fighter* fighter = new Fighter(explosion);
		boids[i]->addComponent(fighter);

		GameObject* engineObj = new GameObject();
		Sound* engine = new Sound("fighterEngine", true, true, 10, true);
		engineObj->addComponent<Sound>(engine);
		boids[i]->addChild(engineObj);

		GameObject::SceneRoot.addChild(boids[i]);
	}
	GameObject* swarm = new GameObject();
	Swarm* swarmComponent = new Swarm(boids, count);
	swarm->addComponent(swarmComponent);
	parent->addChild(swarm);
}


void SceneLoadFunctions::loadFighterWing(GameObject* parent) {
	return loadWing(parent, true);
}

void SceneLoadFunctions::loadEnemyWing(GameObject* parent) {
	return loadWing(parent, false);
}


