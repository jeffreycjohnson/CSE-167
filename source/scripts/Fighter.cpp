#include "Fighter.h"
#include "GameObject.h"
#include "Timer.h"
#include "GPUEmitter.h"
#include "Mesh.h"
#include "Swarm.h"

Fighter::Fighter()
{
	health = 10;
	startTime = 0;
	hideTime = 
	emitterTime = 2;
	killed = false;
}

Fighter::~Fighter()
{
    auto s = dynamic_cast<Swarm*>(swarm);
    if(s)
    {
        s->remove(gameObject);
    }
}

void Fighter::update(float deltaTime)
{
	if (health <= 0 && !killed)
	{
		health = -1;
		killed = true;

		GPUEmitter* emitter = new GPUEmitter(gameObject, "assets/particles/fire1.png", true);
		emitter->minDuration = 1;
		emitter->maxDuration = 2;
		emitter->count = 100;
		emitter->minStartSize = 5;
		emitter->maxStartSize = 7;
		emitter->minEndSize = 7;
		emitter->maxEndSize = 13;
		emitter->minAngularVelocity = -1;
		emitter->maxAngularVelocity = 1;
		emitter->minStartVelocity = { -10, -10, -10 };
		emitter->maxStartVelocity = { 10, 10, 10 };
		emitter->minAcceleration = { 0, 0, 0 };
		emitter->maxAcceleration = { 0, 0, 0 };
		emitter->emitterVelocityScale = 10;
		emitter->rotateTowardsVelocity = false;
		emitter->init();
		emitter->play();
		gameObject->addComponent(emitter);

		startTime = Timer::time();
	}

	if (killed && Timer::time() - startTime > hideTime)
	{
        gameObject->destroy();
		Mesh* mesh = gameObject->getComponent<Mesh>();
		if (mesh != nullptr)
		{
			mesh->visible = false;
		}
		else
		{
			// Get child and try to hide that mesh
		}
	}

	if (killed && Timer::time() - startTime > emitterTime)
	{
		// Kill game object
	}
}

void Fighter::damage(int damage)
{
	health -= damage;
}