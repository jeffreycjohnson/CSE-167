#pragma once
#ifndef INCLUDE_GAME_OBJECT_H
#define INCLUDE_GAME_OBJECT_H

#include "ForwardDecs.h"
#include "Transform.h"
#include "Component.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	Transform transform;
	std::vector<Component*> componentList;


	void addComponent(Component* c) {
		c->setGameObject(this);
		componentList.push_back(c);
	}
};

#endif

