#ifndef INCLUDE_COMPONENT_H
#define INCLUDE_COMPONENT_H

#include "ForwardDecs.h"

class Component
{
	public:
        virtual ~Component() = default;

        GameObject* gameObject;

		virtual void setGameObject(GameObject* go) {
			gameObject = go;
		}
};

#endif