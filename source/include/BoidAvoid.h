#pragma once
#include "Component.h"
#include "GameObject.h"
class BoidAvoid :
	public Component
{
public:
	float radius;
	bool added = false;

	static Component* loadBoidAvoid(GameObject* parent);
	BoidAvoid(float radius);
	~BoidAvoid();
	void update(float dt) override;
};

