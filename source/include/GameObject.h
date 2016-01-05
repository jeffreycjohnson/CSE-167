#ifndef INCLUDE_GAME_OBJECT_H
#define INCLUDE_GAME_OBJECT_H

#include "ForwardDecs.h"
#include "Transform.h"
#include "Component.h"
#include "Material.h"
#include <list>

class GameObject
{
public:
	struct PassList {
		std::list<Component*> forward;
		std::list<Component*> deferred;
		std::list<Component*> particle;
		std::list<Light*> light;
	};

	Transform transform;
	bool dead, visible;

    static GameObject SceneRoot;

	GameObject();
	~GameObject();

    template<typename T>
    void addComponent(T* c) {
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
                delete test;
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

    void addChild(GameObject* go);
	void destroy();
	void hideAll();

	void draw();
	void debugDraw();
    void update(float deltaTime);

	void extract(PassList& list);

	void setMaterial(Material *mat);

	void onCollisionEnter(GameObject* other);

protected:
    std::vector<Component*> componentList;
};

#endif

