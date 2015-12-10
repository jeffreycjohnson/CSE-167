#ifndef INCLUDE_COMPONENT_H
#define INCLUDE_COMPONENT_H

#include "ForwardDecs.h"

class Component
{
	public:
        GameObject* gameObject;
        bool visible = true;

        virtual ~Component() = default;
        virtual void update(float deltaTime);
		virtual void draw();
        virtual void setGameObject(GameObject* go);

		virtual void debugDraw();
		virtual void onCollisionEnter(GameObject* other);
};

#endif