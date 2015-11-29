#ifndef INCLUDE_GAME_OBJECT_H
#define INCLUDE_GAME_OBJECT_H

#include "ForwardDecs.h"
#include "Transform.h"
#include "Component.h"

class GameObject
{
public:
	Transform transform;

    static GameObject SceneRoot;

	GameObject();
	~GameObject();

    template<typename T>
    void GameObject::addComponent(T* c) {
        removeComponent<T>();
        c->setGameObject(this);
        componentList.push_back(c);
    }
    template<typename T>
    bool removeComponent()
    {
        for (auto component = componentList.begin(); component != componentList.end(); ++component)
        {
            T* test = dynamic_cast<T*>(*component);
            if (test)
            {
                test->setGameObject(nullptr);
                componentList.erase(component);
                return true;
            }
        }
        return false;
    }
    template<typename T>
    T* getComponent()
    {
        for(auto component : componentList)
        {
            T* ret = dynamic_cast<T*>(component);
            if (ret) return ret;
        }
        return nullptr;
    }

    void addChild(GameObject& go);

	void draw();
    void update(float deltaTime);
protected:
    std::vector<Component*> componentList;
};

#endif

