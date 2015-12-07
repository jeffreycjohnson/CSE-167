#include "PlayerController.h"
#include "GameObject.h"
#include "Input.h"
#include "MathFunc.h"

PlayerController::PlayerController()
{
	speed = 0;
	topSpeed = 1;
	rotationSensitivity = 2;
	thrustSensitivity = 1;
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

	speed += Input::getAxis("thrust") * thrustSensitivity * deltaTime;
	if (speed > topSpeed)
		speed = topSpeed;
	else if (speed < 0)
		speed = 0;

	forward = glm::normalize(glm::mat3(gameObject->transform.getTransformMatrix()) * glm::vec3(0, 0, -1)) * speed;
	currentVel += (forward - currentVel) * (1 + thrustSensitivity * topSpeed - speed) * deltaTime;
	gameObject->transform.translate(currentVel);

	glm::quat rollDelta = glm::angleAxis(currentRot.x, glm::vec3(0, 0, -1));
	glm::quat yawDelta = glm::angleAxis(currentRot.y, glm::vec3(0, -1, 0));
	glm::quat pitchDelta = glm::angleAxis(currentRot.z, glm::vec3(1, 0, 0));

	gameObject->transform.rotate(rollDelta * yawDelta * pitchDelta);
}