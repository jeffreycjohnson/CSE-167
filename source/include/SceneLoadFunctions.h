#include "Component.h"
#include "GameObject.h"

#pragma once
class EmptyComponent : public Component
{
};

class SceneLoadFunctions {

public:
	static Component* loadPlayer(GameObject* parent);
	static Component* loadAsteroidBoid(GameObject* parent);
	static Component* loadAsteroidNoBoid(GameObject* parent);
	static Component* loadCapitalShip(GameObject* parent);
	static Component* loadFighterWing(GameObject* parent);
	static Component* loadEnemyWing(GameObject* parent);
};

