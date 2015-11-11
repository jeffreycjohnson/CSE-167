#include "GameObject.h"

GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}

void GameObject::addChild(GameObject& go) {
    transform.children.push_back(std::shared_ptr<Transform>(&go.transform));
    go.transform.parent = &transform;
}