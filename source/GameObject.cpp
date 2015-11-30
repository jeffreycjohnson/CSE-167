#include "GameObject.h"
#include "Mesh.h"
#include "GPUEmitter.h"
#include "Renderer.h"

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
		Renderer::setModelMatrix(transform.getTransformMatrix());
		mesh->draw();
	}
	GPUEmitter* emitter;
	if ((emitter = getComponent<GPUEmitter>()) != nullptr) {
		Renderer::setModelMatrix(transform.getTransformMatrix());
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