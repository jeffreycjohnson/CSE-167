#ifndef INCLUDE_FIGHTER_H
#define INCLUDE_FIGHTER_H

#include "Component.h"

class Fighter : public Component
{
private:
	int health;
	float startTime, hideTime, emitterTime;
	bool killed;

public:
	Fighter();
	~Fighter();

	void update(float deltaTime);
	void damage(int damage);
};

#endif