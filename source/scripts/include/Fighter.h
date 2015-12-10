#ifndef INCLUDE_FIGHTER_H
#define INCLUDE_FIGHTER_H

#include "Component.h"
#include "Swarm.h"

class Fighter : public Component
{
private:
	int health;
	float startTime, hideTime, emitterTime;
	bool killed;
public:
    Component* swarm;
	Fighter();
	~Fighter();

	void update(float deltaTime);
	void damage(int damage);
};

#endif