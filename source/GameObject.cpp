#include "GameObject.h"
#include "Mesh.h"
#include "GPUEmitter.h"
#include "Renderer.h"
#include "Light.h"
#include "ParticleTrail.h"
#include "BoxCollider.h"

GameObject GameObject::SceneRoot;

GameObject::GameObject() {
	transform.setGameObject(this);
	dead = false;
	visible = true;
}

GameObject::~GameObject() {
	for (auto child : transform.children) {
		if(child && child->gameObject) delete child->gameObject;
	}
	for (auto component : componentList) {
		if(component) delete component;
	}
}

void GameObject::addChild(GameObject& go) {
    transform.children.push_back(&go.transform);
    go.transform.parent = &transform;
    go.transform.transformMatrixDirty = true;
}

void GameObject::destroy() {
	dead = true;
}

void GameObject::draw() {
	if (visible) {
		for (auto component : componentList) {
			if (component->visible)
				component->draw();
		}
		for (auto child : transform.children) {
			(child->gameObject)->draw();
		}
	}
}

void GameObject::debugDraw() {
	for (auto component : componentList) {
		component->debugDraw();
	}
	for (auto child : transform.children) {
		(child->gameObject)->debugDraw();
	}
}

void GameObject::update(float deltaTime)
{
	Transform* object;
	for (int i = 0; i < transform.children.size(); i++)
    {
		object = transform.children[i];
		if (object->gameObject->dead)
		{
			BoxCollider* collider;
			if ((collider = getComponent<BoxCollider>()) != nullptr) {
				collider->remove();
			}
			delete object->gameObject;
			transform.children.erase(transform.children.begin() + i);
		}
		else
			object->gameObject->update(deltaTime);
    }
    for (auto component : componentList)
    {
        component->update(deltaTime);
    }
}

void GameObject::extract(PassList & list)
{
	Mesh* mesh;
	if ((mesh = getComponent<Mesh>()) != nullptr) {
		if (mesh->material && mesh->material->transparent) {
			list.forward.push_back(mesh);
		}
        else
        {
            list.deferred.push_back(mesh);
        }
	}
	GPUEmitter* emitter;
	if ((emitter = getComponent<GPUEmitter>()) != nullptr) {
		list.particle.push_back(emitter);
	}
	ParticleTrail* trail;
	if ((trail = getComponent<ParticleTrail>()) != nullptr) {
		list.particle.push_back(trail);
	}
	Light* light;
	if ((light = getComponent<Light>()) != nullptr) {
		list.light.push_back(light);
	}

	for (auto child : transform.children) {
		(child->gameObject)->extract(list);
	}
}

void GameObject::setMaterial(Material *mat) {
	Mesh* mesh;
	if ((mesh = getComponent<Mesh>()) != nullptr) {
		mesh->setMaterial(mat);
	}
	for (auto child : transform.children) {
		(child->gameObject)->setMaterial(mat);
	}
}

void GameObject::onCollisionEnter(GameObject* other)
{
	if (this != nullptr)
	{
		for (int i = 0; i < componentList.size(); i++)
		{
			componentList[i]->onCollisionEnter(other);
		}
	}
	else
	{
		// Weird shit happened and code will sometimes run here
	}
}