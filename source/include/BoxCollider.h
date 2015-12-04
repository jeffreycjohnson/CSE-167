#ifndef INCLUDE_BOX_COLLIDER_H
#define INCLUDE_BOX_COLLIDER_H

#include "Component.h"

class BoxCollider : public Component
{
	BoxCollider();
	~BoxCollider();
	void update(float deltaTime);
	void draw();
	void setGameObject(GameObject* go);
};

#endif