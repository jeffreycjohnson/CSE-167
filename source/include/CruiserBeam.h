#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Sound.h"

class CruiserBeam :
	public Component
{
public:
	bool firing = false;
	float fireTime = 0;
	float laserDuration = 3.5;
	float coolTime = 0;
	float coolDuration = 12;
	float shake = 0.05f;
	glm::vec3 startScale;

	GameObject* laser;
	Sound* boom;

	static void loadCruiserBeam(GameObject* parent);
	CruiserBeam(GameObject*);
	~CruiserBeam();
	void update(float) override;
};

