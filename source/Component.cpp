#include "Component.h"

void Component::update(float deltaTime){}
void Component::draw() {}
void Component::setGameObject(GameObject* go) {
    gameObject = go;
}
void Component::debugDraw() {}
void Component::onCollisionEnter(GameObject* other) {}