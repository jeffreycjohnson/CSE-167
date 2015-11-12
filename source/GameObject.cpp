#include "GameObject.h"
#include "Mesh.h"
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
	for (auto child : transform.children) {
		(child->gameObject)->draw();
	}
}