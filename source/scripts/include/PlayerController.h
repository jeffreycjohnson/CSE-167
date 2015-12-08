#ifndef INCLUDE_PLAYER_CONTROLLER_H
#define INCLUDE_PLAYER_CONTROLLER_H

#include "Component.h"

#define PLAYER_SPEED_DAMPING 0.01f
#define PLAYER_ROTATION_DAMPING 0.1f
#define ROLL_SPEED 2.0f

class PlayerController : public Component
{
private:
	glm::vec3 currentRot, currentVel, targetVel, forward;
	float speed, topSpeed, boostSpeed, maxBoost, rotationSensitivity, thrustSensitivity, startFOV;

public:
	PlayerController();
	~PlayerController();

	void update(float deltaTime);
};

#endif