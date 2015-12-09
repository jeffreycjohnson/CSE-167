#pragma once
#include "Component.h"
#include "GameObject.h"
class Turret :
	public Component
{
public:
	static void loadTurret(GameObject* parent);
	Turret();
	~Turret();
	void update(float dt) override;
};

