#include "BoidAvoid.h"
#include "Swarm.h"
#include "Mesh.h"

Component* BoidAvoid::loadBoidAvoid(GameObject* parent)
{
	 parent->removeComponent<Mesh>();
	 BoidAvoid* addComponent = new BoidAvoid(parent->transform.scaleFactor.x);
	 return addComponent;
}

BoidAvoid::BoidAvoid(float radius) : radius(radius) {
}


BoidAvoid::~BoidAvoid()
{
}


void BoidAvoid::update(float dt) {
	if (!added) {
		BoidSphere* obstacle = new BoidSphere();
		obstacle->id = 0;
		obstacle->transform = &(gameObject->transform);
		obstacle->radius = radius;
		Swarm::addObstacle(obstacle);
		added = true;
	}
}
