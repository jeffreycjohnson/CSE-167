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

	void addChild(GameObject& go) {
		transform.children.push_back(std::shared_ptr<Transform>(&go.transform));
		go.transform.parent = &transform;
	}
};

#endif

