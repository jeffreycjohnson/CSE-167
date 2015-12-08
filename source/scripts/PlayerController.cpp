#include "PlayerController.h"
#include "GameObject.h"
#include "Input.h"
#include "MathFunc.h"
#include "Renderer.h"
#include "Camera.h"
#include "Bullet.h"
#include "GPUEmitter.h"
#include "BoxCollider.h"
#include "ObjectLoader.h"
#include "Timer.h"

PlayerController::PlayerController()
{
	speed = boostSpeed = 0;
	topSpeed = 1;
	maxBoost = 0.15f;
	rotationSensitivity = 2;
	thrustSensitivity = 1;
	startFOV = Renderer::camera->fov;
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
		makeBullet(true);
		makeBullet(false);
		Renderer::camera->screenShake(0.025, 0.2);
		startShoot = Timer::time();
	}

	bool zoom = Input::getButton("aim");
	if (zoom)
		Renderer::camera->fov = startFOV - 0.5f;
	else
		Renderer::camera->fov = startFOV;

	float afterburner = (Input::getAxis("afterburner") - 0.5f) * 2.0f;
	boostSpeed += afterburner * thrustSensitivity * deltaTime;
	if (boostSpeed > maxBoost)
		boostSpeed = maxBoost;
	else if (boostSpeed < 0)
		boostSpeed = 0;

	float currentSpeed = speed + boostSpeed;
	if (afterburner >= 0)
		Renderer::camera->screenShake(currentSpeed * afterburner / 100, 2);
	else
		afterburner = 0;

	speed += Input::getAxis("thrust") * thrustSensitivity * deltaTime;
	if (speed > topSpeed)
		speed = topSpeed;
	else if (speed < 0)
		speed = 0;

	forward = glm::normalize(glm::mat3(gameObject->transform.getTransformMatrix()) * glm::vec3(0, 0, -1));
	currentVel += (1 + afterburner) * (forward * currentSpeed - currentVel) * (1 + thrustSensitivity * topSpeed - currentSpeed) * deltaTime;
	gameObject->transform.translate(currentVel);

	glm::quat rollDelta = glm::angleAxis(currentRot.x, glm::vec3(0, 0, -1));
	glm::quat yawDelta = glm::angleAxis(currentRot.y, glm::vec3(0, -1, 0));
	glm::quat pitchDelta = glm::angleAxis(currentRot.z, glm::vec3(1, 0, 0));

	gameObject->transform.rotate(rollDelta * yawDelta * pitchDelta);
}

void PlayerController::makeBullet(bool side)
{
	GameObject* bulletObj = loadScene("assets/sphere.obj");
	glm::vec3 offset(4, 1, 5);
	if (side)
	{
		offset.x = -offset.x;
	}
	Bullet* bullet = new Bullet(gameObject->transform.getWorldPosition() + glm::mat3(gameObject->transform.getTransformMatrix()) * offset, forward, currentVel, 2);
	bulletObj->addComponent(bullet);
	bullet->init();
	BoxCollider* collider = new BoxCollider({ 0, 0, 0 }, { 1, 1, 5 });
	collider->passive = false;
	bulletObj->addComponent(collider);
	GameObject::SceneRoot.addChild(*bulletObj);
}