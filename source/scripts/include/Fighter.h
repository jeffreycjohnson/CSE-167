#ifndef INCLUDE_FIGHTER_H
#define INCLUDE_FIGHTER_H

#include "Component.h"

class Fighter : public Component
{
private:
	int health;

public:
	Fighter();
	~Fighter();

	void update(float deltaTime);
	void damage(int damage);
};

#endif