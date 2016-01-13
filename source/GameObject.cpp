#include "GameObject.h"
#include "Mesh.h"
#include "GPUEmitter.h"
#include "Light.h"
#include "ParticleTrail.h"
#include "BoxCollider.h"

GameObject GameObject::SceneRoot;
std::multimap<std::string, GameObject*> GameObject::nameMap;

GameObject * GameObject::FindByName(const std::string& name)
{
    auto iter = nameMap.find(name);
    return iter == nameMap.end() ? nullptr : iter->second;
}

std::vector<GameObject*> GameObject::FindAllByName(const std::string& name)
{
    std::vector<GameObject*> ret;
    auto range = nameMap.equal_range(name);
    while (range.first != range.second) {
        ret.push_back(range.first->second);
        ++range.first;
    }
    return ret;
}

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
    removeName();
}

void GameObject::addChild(GameObject* go) {
    transform.children.push_back(&go->transform);
    go->transform.parent = &transform;
}

void GameObject::destroy() {
	dead = true;
}

void GameObject::hideAll()
{
	Mesh* mesh = getComponent<Mesh>();
	if (mesh != nullptr)
		mesh->visible = false;

	ParticleTrail* trail = getComponent<ParticleTrail>();
	if (trail != nullptr)
		trail->emitting = false;

	for (auto child : transform.children)
	{
		child->gameObject->hideAll();
	}
}

bool GameObject::isChildOf(GameObject* go) const
{
    auto parent = transform.parent;
    while(parent)
    {
        if (parent->gameObject == go) return true;
        parent = parent->parent;
    }
    return false;
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
	for (unsigned int i = 0; i < transform.children.size(); i++)
    {
        auto object = transform.children[i];
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

void GameObject::extract(PassList& list)
{
	if (visible) {
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
	}

	for (auto child : transform.children) {
		(child->gameObject)->extract(list);
	}
}

void GameObject::setMaterial(Material* mat) {
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
	for (auto component : componentList)
	{
		component->onCollisionEnter(other);
	}
}

void GameObject::setName(const std::string& name)
{
    removeName();
    nameMap.insert(std::make_pair(name, this));
}

std::string GameObject::getName() const
{
    return name;
}

void GameObject::removeName()
{
    auto range = nameMap.equal_range(name);
    while (range.first != range.second)
    {
        if (range.first->second == this) {
            nameMap.erase(range.first);
            return;
        }
        ++range.first;
    }
}
