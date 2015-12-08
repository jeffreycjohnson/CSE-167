#include "PlayerController.h"
#include "GameObject.h"
#include "Input.h"
#include "MathFunc.h"
#include "Renderer.h"
#include "Camera.h"
#include "Bullet.h"

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

	float zoom = Input::getAxis("aim");
	if (zoom >= 0)
	{
		Renderer::camera->fov = startFOV - zoom / 2;
	}

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

	forward = glm::normalize(glm::mat3(gameObject->transform.getTransformMatrix()) * glm::vec3(0, 0, -1)) * currentSpeed;
	currentVel += (1 + afterburner) * (forward - currentVel) * (1 + thrustSensitivity * topSpeed - currentSpeed) * deltaTime;
	gameObject->transform.translate(currentVel);

	glm::quat rollDelta = glm::angleAxis(currentRot.x, glm::vec3(0, 0, -1));
	glm::quat yawDelta = glm::angleAxis(currentRot.y, glm::vec3(0, -1, 0));
	glm::quat pitchDelta = glm::angleAxis(currentRot.z, glm::vec3(1, 0, 0));

	gameObject->transform.rotate(rollDelta * yawDelta * pitchDelta);
}