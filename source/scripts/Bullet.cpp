#include "Bullet.h"
#include "GameObject.h"
#include "Fighter.h"
#include "GPUEmitter.h"
#include "BoxCollider.h"
#include "Mesh.h"

Bullet::Bullet(glm::vec3 startPos, glm::vec3 forward)
{
	this->startPos = startPos;
	this->forward = forward;
}

Bullet::~Bullet()
{

}

void Bullet::update(float deltaTime)
{

}

void Bullet::onCollisionEnter(GameObject* other)
{
	Fighter* fighter = other->getComponent<Fighter>();
	if (fighter != nullptr)
	{
		fighter->damage(1);
	}

	GPUEmitter* emitter = gameObject->getComponent<GPUEmitter>();
	if (emitter != nullptr)
	{
		emitter->play();
		// Start wait timer before destroying
	}

	// Prevent bullet from interacting with anything else
	BoxCollider* collider = gameObject->getComponent<BoxCollider>();
	if (collider != nullptr)
	{
		collider->gameObject->removeComponent<BoxCollider>();
	}
	Mesh* mesh = gameObject->getComponent<Mesh>();
	if (mesh != nullptr)
	{
		mesh->gameObject->removeComponent<Mesh>();
	}
}