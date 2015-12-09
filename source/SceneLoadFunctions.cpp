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

Component* loadX(GameObject* parent, std::string filename) {
	parent->removeComponent<Mesh>();

	GameObject* childObj = loadScene(filename);
	childObj->transform.rotate(glm::angleAxis(atanf(1)*2.f, glm::vec3(1, 0, 0))); //convert back to z-up axis
	parent->addChild(*childObj);

	Component* addComponent = new EmptyComponent();
	return addComponent;
}

Component* SceneLoadFunctions::loadPlayer(GameObject* parent) {

	GameObject* camera = loadScene("assets/cockpit.fbx");
	parent->addChild(*camera);
	camera->addComponent(Renderer::camera);
	camera->transform.setPosition(0, 0, 20);
	camera->transform.getWorldPosition(); // Force matrix updates to prevent loud initial sounds
	Renderer::camera->getCameraMatrix();
	camera->update(0);

	Sound* camSound = new Sound("cabin", true, true, 0.75f);
	camera->addComponent(camSound);

	PlayerController* controller = new PlayerController();
	camera->addComponent(controller);

	GameObject* musicObj = new GameObject();
	Sound* music = new Sound("music", true, true, 0.25f);
	musicObj->addComponent(music);
	parent->addChild(*musicObj);

	BoidAvoid* obstacle = new BoidAvoid(5);
	parent->addComponent(obstacle);

	return new EmptyComponent;
}

Component* SceneLoadFunctions::loadAsteroidBoid(GameObject* parent) {
	loadX(parent, "assets/asteroid3.fbx");
	return new BoidAvoid(1);
}

Component* SceneLoadFunctions::loadAsteroidNoBoid(GameObject* parent) {
	return loadX(parent, "assets/asteroid3.fbx");
}

Component* SceneLoadFunctions::loadCapitalShip(GameObject* parent) {
	return loadX(parent, "assets/cruiserPrototype.fbx");
}

Component* loadWing(GameObject* parent, bool allied) {

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
		boids[i]->addChild(*boid);
		boids[i]->transform.scale(1);
		boids[i]->transform.translate(parent->transform.getWorldPosition());

		BoxCollider* collider = new BoxCollider(glm::vec3(0, 3, 2), glm::vec3(5, 7, 9));
		boids[i]->addComponent(collider);

		ParticleTrail* trail = new ParticleTrail();
		trail->material = trailMaterial;
		boids[i]->addComponent<ParticleTrail>(trail);

		Fighter* fighter = new Fighter();
		boids[i]->addComponent(fighter);

		GameObject::SceneRoot.addChild(*boids[i]);
	}
	GameObject* swarm = new GameObject();
	Swarm* swarmComponent = new Swarm(boids, count);
	swarm->addComponent(swarmComponent);
	parent->addChild(*swarm);
	return new EmptyComponent();
}


Component* SceneLoadFunctions::loadFighterWing(GameObject* parent) {
	return loadWing(parent, true);
}

Component* SceneLoadFunctions::loadEnemyWing(GameObject* parent) {
	return loadWing(parent, false);
}


