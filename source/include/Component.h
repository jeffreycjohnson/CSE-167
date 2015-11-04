#ifndef INCLUDE_COMPONENT_H
#define INCLUDE_COMPONENT_H

#include "ForwardDecs.h"

class Component
{
	public:
		GameObject* gameObject;

		void setGameObject(GameObject* go) {
			gameObject = go;
		}
};

#endif