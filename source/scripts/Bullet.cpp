#include "Bullet.h"
#include "GameObject.h"
#include "Fighter.h"
#include "GPUEmitter.h"
#include "BoxCollider.h"
#include "Mesh.h"
#include "Timer.h"
#include "ObjectLoader.h"

Bullet::Bullet(glm::vec3 startPos, glm::vec3 forward, glm::vec3 starVel, float emitterDuration = 0)
{
	this->startPos = startPos;
   	this->startVel = startVel;
	this->forward = forward;
	this->emitterDuration = emitterDuration;
	dying = false;
}

Bullet::~Bullet()
{

}

void Bullet::init()
{
	gameObject->transform.setPosition(startPos);
}

void Bullet::update(float deltaTime)
{
	gameObject->transform.translate((startVel + forward * BULLET_VELOCITY) * deltaTime);
	if (!dying && glm::length(gameObject->transform.getWorldPosition() - startPos) > MAX_BULLET_DISTANCE)
		destroy();

	if (dying && Timer::time() - startTime > emitterDuration)
		destroy();
}

void Bullet::onCollisionEnter(GameObject* other)
{
	if (other != nullptr && other->getComponent<Bullet>() == nullptr)
	{
		Fighter* fighter = other->getComponent<Fighter>();
		if (fighter != nullptr)
		{
			fighter->damage(1);
		}


		GPUEmitter* emitter = new GPUEmitter(gameObject, "assets/particles/spark.jpg", true);
		emitter->minDuration = 1;
		emitter->maxDuration = 2;
		emitter->count = 100;
		emitter->minStartVelocity = { -1, -1, -1 };
		emitter->maxStartVelocity = { 1, 1, 1 };
		emitter->minAcceleration = { 0, 0, 0 };
		emitter->maxAcceleration = { 0, 0, 0 };
		emitter->emitterVelocityScale = 10;
		emitter->init();
		gameObject->addComponent(emitter);
		emitter->emitterVelocity = gameObject->transform.getWorldPosition() - other->transform.getWorldPosition();
		emitter->play();
		startTime = Timer::time();
		dying = true;

		// Prevent bullet from interacting with anything else
		BoxCollider* collider = gameObject->getComponent<BoxCollider>();
		if (collider != nullptr)
		{
			//collider->gameObject->removeComponent<BoxCollider>();
		}

		// Turn invisible as it waits for the emitter to finish
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
}

void Bullet::destroy()
{
	gameObject->destroy();
}