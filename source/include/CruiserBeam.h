#pragma once
#include "Component.h"
#include "GameObject.h"

class CruiserBeam :
	public Component
{
public:
	bool firing = false;
	float fireTime = 0;
	float laserDuration = 5.5;
	float coolTime = 0;
	float coolDuration = 12;

	GameObject* laser;

	static void loadCruiserBeam(GameObject* parent);
	CruiserBeam(GameObject*);
	~CruiserBeam();
	void update(float) override;
};

