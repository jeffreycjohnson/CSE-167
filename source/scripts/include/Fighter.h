#ifndef INCLUDE_FIGHTER_H
#define INCLUDE_FIGHTER_H

#include "Component.h"
#include "Swarm.h"
#include "Sound.h"

class Fighter : public Component
{
private:
	Sound* explosion;
	int health;
	float startTime, hideTime, emitterTime;
	bool killed;
public:
    Component* swarm;
	Fighter(Sound* explosion);
	~Fighter();

	void update(float deltaTime);
	void damage(int damage);
};

#endif