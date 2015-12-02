#ifndef INCLUDE_COMPONENT_H
#define INCLUDE_COMPONENT_H

#include "ForwardDecs.h"

class Component
{
	public:
        GameObject* gameObject;

        virtual ~Component() = default;
        virtual void update(float deltaTime);
		virtual void draw();
        virtual void setGameObject(GameObject* go);
};

#endif