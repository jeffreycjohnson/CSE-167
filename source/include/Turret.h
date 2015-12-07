#pragma once
#include "Component.h"
#include "GameObject.h"
class Turret :
	public Component
{
public:
	static Component* loadTurret(GameObject* parent);
	Turret();
	~Turret();
};

