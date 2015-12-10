#ifndef INCLUDE_BULLET_H
#define INCLUDE_BULLET_H

#include "Component.h"

#define MAX_BULLET_DISTANCE 500.0f
#define BULLET_VELOCITY 500.0f

class Bullet : public Component
{
private:
	float startTime, emitterDuration;
	bool dying;

public:
	glm::vec3 startPos, startVel, forward;
	Bullet(glm::vec3 startPos, glm::vec3 forward, glm::vec3 starVel, float emitterDuration);
	~Bullet();

	void init();
	void update(float deltaTime);
	void onCollisionEnter(GameObject* other);
	void destroy();
};

#endif