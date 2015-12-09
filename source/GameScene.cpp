#include "GameScene.h"
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
#include "SceneLoadFunctions.h"


GameScene::GameScene()
{

	addComponentMapping("boidAvoid", BoidAvoid::loadBoidAvoid);
	addComponentMapping("TurretPlaceholder", Turret::loadTurret);

	addComponentMapping("AsteroidPlaceholder", SceneLoadFunctions::loadAsteroidBoid);
	addComponentMapping("AsteroidCluster", SceneLoadFunctions::loadAsteroidNoBoid);
	addComponentMapping("CruiserPlaceholder", SceneLoadFunctions::loadCapitalShip);
	addComponentMapping("EnemyWing", SceneLoadFunctions::loadEnemyWing);
	addComponentMapping("AlliedWing", SceneLoadFunctions::loadFighterWing);
	//addComponentMapping("PlayerPlaceholder", SceneLoadFunctions::loadPlayer);

	sun = new GameObject();
	auto sunLight = new DirectionalLight(true);
	sunLight->color = glm::vec3(0.5, 0.5, 0.5);
	sun->addComponent(sunLight);
	sun->transform.translate(5, 8.5, -1.5);
	sun->transform.setRotate(glm::quat_cast(glm::orientation(glm::vec3(0.5, 0.85, -0.15), glm::vec3(0, 1, 0))));
	GameObject::SceneRoot.addChild(*sun);

	GameObject::SceneRoot.addChild(*loadScene("assets/gameScene.fbx"));




	GameObject* camera = loadScene("assets/cockpit.fbx");
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
	camera->addChild(*musicObj);

	BoidAvoid* obstacle = new BoidAvoid(5);
	camera->addComponent(obstacle);

	GameObject::SceneRoot.addChild(*camera);
}


GameScene::~GameScene()
{
}


void GameScene::loop() {

	BoxCollider::updateColliders();

	if (Input::getMouseDown("mouse 0"))
		Renderer::camera->screenShake(0.01, 0.25);
//	if (Input::getKeyDown("space"))
//		emitter->getComponent<GPUEmitter>()->play();
//	if (Input::getMouseDown("mouse 0"))
//		bear->getComponent<Sound>()->play();
//	if (Input::getKeyDown("left control"))
//		camera->getComponent<Sound>()->toggle();
}