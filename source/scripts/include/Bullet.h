#ifndef INCLUDE_BULLET_H
#define INCLUDE_BULLET_H

#include "Component.h"

#define MAX_BULLET_DISTANCE 200.0f
#define BULLET_VELOCITY 1000.0f

class Bullet : public Component
{
private:
	glm::vec3 startPos, forward;

public:
	Bullet(glm::vec3 startPos, glm::vec3 forward);
	~Bullet();

	void update(float deltaTime);
	void onCollisionEnter(GameObject* other);
};

#endif