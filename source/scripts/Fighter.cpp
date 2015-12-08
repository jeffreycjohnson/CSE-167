#include "Fighter.h"

Fighter::Fighter()
{

}

Fighter::~Fighter()
{

}

void Fighter::update(float deltaTime)
{
	if (health <= 0)
	{
		// Explode and destroy self
	}
}

void Fighter::damage(int damage)
{
	health -= damage;
}