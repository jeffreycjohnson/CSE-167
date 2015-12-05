#include "GameObject.h"
#include "Mesh.h"
#include "GPUEmitter.h"
#include "Renderer.h"
#include "Light.h"
#include "ParticleTrail.h"

GameObject GameObject::SceneRoot;

GameObject::GameObject() {
	transform.setGameObject(this);
}

GameObject::~GameObject() {
	for (auto child : transform.children) {
		delete (child)->gameObject;
	}
}

void GameObject::addChild(GameObject& go) {
    transform.children.push_back(&go.transform);
    go.transform.parent = &transform;
    go.transform.transformMatrixDirty = true;
}

void GameObject::draw() {
	for (auto component : componentList) {
		component->draw();
	}
	for (auto child : transform.children) {
		(child->gameObject)->draw();
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
    for(auto object : transform.children)
    {
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
	for (int i = 0; i < componentList.size(); i++)
	{
		componentList[i]->onCollisionEnter(other);
	}
}