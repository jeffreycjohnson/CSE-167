#include "Component.h"
#include "GameObject.h"

#pragma once
class EmptyComponent : public Component
{
};

class SceneLoadFunctions {

public:
	static void loadPlayer(GameObject* parent);
	static void loadAsteroidBoid(GameObject* parent);
	static void loadAsteroidNoBoid(GameObject* parent);
	static void loadCapitalShip(GameObject* parent);
	static void loadFighterWing(GameObject* parent);
	static void loadEnemyWing(GameObject* parent);
};

