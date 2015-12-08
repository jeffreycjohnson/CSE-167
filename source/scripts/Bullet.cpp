#include "Bullet.h"
#include "GameObject.h"
#include "Fighter.h"
#include "GPUEmitter.h"
#include "BoxCollider.h"
#include "Mesh.h"
#include "Timer.h"

Bullet::Bullet(glm::vec3 startPos, glm::vec3 forward, float emitterDuration = 0)
{
	this->startPos = startPos;
	this->forward = forward;
	this->emitterDuration = 0;
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
	gameObject->transform.translate(forward * BULLET_VELOCITY * deltaTime);
	if (glm::length(gameObject->transform.position - startPos) > MAX_BULLET_DISTANCE)
		destroy();

	if (dying && Timer::time() - startTime > emitterDuration)
		destroy();
}

void Bullet::onCollisionEnter(GameObject* other)
{
	if (other != nullptr)
	{
		Fighter* fighter = other->getComponent<Fighter>();
		if (fighter != nullptr)
		{
			fighter->damage(1);
		}
	}

	GPUEmitter* emitter = gameObject->getComponent<GPUEmitter>();
	if (emitter != nullptr)
	{
		emitter->play();
		startTime = Timer::time();
		dying = true;
	}
	else // No need to wait
	{
		destroy();
	}

	// Prevent bullet from interacting with anything else
	BoxCollider* collider = gameObject->getComponent<BoxCollider>();
	if (collider != nullptr)
	{
		collider->gameObject->removeComponent<BoxCollider>();
	}

	// Turn invisible as it waits for the emitter to finish
	Mesh* mesh = gameObject->getComponent<Mesh>();
	if (mesh != nullptr)
	{
		mesh->gameObject->removeComponent<Mesh>();
	}
}

void Bullet::destroy()
{
	gameObject->destroy();
}