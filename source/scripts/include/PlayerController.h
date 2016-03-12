#ifndef INCLUDE_PLAYER_CONTROLLER_H
#define INCLUDE_PLAYER_CONTROLLER_H

#include "Component.h"
#include "Sound.h"

#define PLAYER_SPEED_DAMPING 0.01f
#define PLAYER_ROTATION_DAMPING 0.1f
#define ROLL_SPEED 2.0f
#define SHOOT_INTERVAL 0.1f

class PlayerController : public Component
{
private:
	glm::vec3 currentRot, currentVel, targetVel, forward;
	float speed, topSpeed, boostSpeed, startShoot, maxBoost, rotationSensitivity, thrustSensitivity, startFOV;
	Sound* gunSound, *boostSound;

	void makeBullet(bool side);

public:
	PlayerController(Sound* gun, Sound* boost);
	~PlayerController();

	void update(float deltaTime) override;
};

#endif