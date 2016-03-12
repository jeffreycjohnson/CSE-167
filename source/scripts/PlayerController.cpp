#include "PlayerController.h"
#include "GameObject.h"
#include "Input.h"
#include "Renderer.h"
#include "Camera.h"
#include "Bullet.h"
#include "BoxCollider.h"
#include "ObjectLoader.h"
#include "Timer.h"
#include "Sound.h"

PlayerController::PlayerController(Sound* gun, Sound* boost)
{
	gunSound = gun;
	boostSound = boost;
	speed = boostSpeed = 0;
	topSpeed = 1;
	maxBoost = 0.15f;
	rotationSensitivity = 2;
	thrustSensitivity = 1;
	startFOV = Renderer::mainCamera->fov;
}

PlayerController::~PlayerController()
{

}

void PlayerController::update(float deltaTime)
{
	glm::vec3 rotationInputs;
	rotationInputs.x = Input::getAxis("roll") * rotationSensitivity * ROLL_SPEED * deltaTime;
	rotationInputs.y = Input::getAxis("yaw") * rotationSensitivity * deltaTime;
	rotationInputs.z = Input::getAxis("pitch") * rotationSensitivity * deltaTime;
	currentRot += (rotationInputs - currentRot) * PLAYER_ROTATION_DAMPING;

	if ((Input::getButton("fire") || Input::getAxis("fire") > 0.5f) && Timer::time() - startShoot > SHOOT_INTERVAL)
	{
		gunSound->play();
		makeBullet(true);
		Renderer::mainCamera->screenShake(0.025, 0.2);
		startShoot = Timer::time();
	}

	bool zoom = Input::getButton("aim");
	if (zoom)
		Renderer::mainCamera->fov = startFOV - 0.5f;
	else
		Renderer::mainCamera->fov = startFOV;

	float afterburner = (Input::getAxis("afterburner") - 0.5f) * 2.0f;
	boostSpeed += afterburner * thrustSensitivity * deltaTime;
	if (boostSpeed > maxBoost)
		boostSpeed = maxBoost;
	else if (boostSpeed < 0)
		boostSpeed = 0;

	float currentSpeed = speed + boostSpeed;
	if (afterburner >= 0)
		Renderer::mainCamera->screenShake(currentSpeed * afterburner / 100, 2);
	else
		afterburner = 0;

	speed += Input::getAxis("thrust") * thrustSensitivity * deltaTime;
	if (speed > topSpeed)
		speed = topSpeed;
	else if (speed < 0)
		speed = 0;

	boostSound->setVolume(speed / topSpeed * 0.2f + boostSpeed / maxBoost  * 0.4f);

	forward = glm::normalize(glm::mat3(gameObject->transform.getTransformMatrix()) * glm::vec3(0, 0, -1));
	currentVel += (1 + afterburner) * (forward * currentSpeed - currentVel) * (1 + thrustSensitivity * topSpeed - currentSpeed) * deltaTime;
	gameObject->transform.translate(currentVel * deltaTime * 60.f);

	glm::quat rollDelta = glm::angleAxis(currentRot.x, glm::vec3(0, 0, -1));
	glm::quat yawDelta = glm::angleAxis(currentRot.y, glm::vec3(0, -1, 0));
	glm::quat pitchDelta = glm::angleAxis(currentRot.z, glm::vec3(1, 0, 0));

	gameObject->transform.rotate(rollDelta * yawDelta * pitchDelta);
}

void PlayerController::makeBullet(bool side)
{
	GameObject* bulletObj = loadScene("assets/bullet.fbx");
	glm::vec3 offset(0, -1, -3);
	if (side)
	{
		offset.x = -offset.x;
	}
	Bullet* bullet = new Bullet(gameObject->transform.getWorldPosition() + glm::mat3(gameObject->transform.getTransformMatrix()) * offset, 
		glm::normalize(glm::mat3(gameObject->transform.getTransformMatrix()) * glm::normalize(glm::vec3(0, 0.05f, -1))), currentVel, 2);
	bulletObj->addComponent(bullet);
    bulletObj->transform.setRotate(gameObject->transform.getRotation());
	bullet->init();
	BoxCollider* collider = new BoxCollider({ 0, 0, 0 }, { 1, 1, 5 });
	collider->passive = false;
	bulletObj->addComponent(collider);
	GameObject::SceneRoot.addChild(bulletObj);
}
