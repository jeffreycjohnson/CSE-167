#include "GameObject.h"
#include "Mesh.h"
#include "GPUEmitter.h"
#include "Renderer.h"
#include "Light.h"

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
}

void GameObject::draw() {
	Mesh* mesh;
	if ((mesh = getComponent<Mesh>()) != nullptr) {
		mesh->draw();
	}
	GPUEmitter* emitter;
	if ((emitter = getComponent<GPUEmitter>()) != nullptr) {
		emitter->draw();
	}
	for (auto child : transform.children) {
		(child->gameObject)->draw();
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
		list.forward.push_back(mesh);
	}
	GPUEmitter* emitter;
	if ((emitter = getComponent<GPUEmitter>()) != nullptr) {
		list.particle.push_back(emitter);
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