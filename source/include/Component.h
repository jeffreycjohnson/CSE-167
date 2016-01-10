#ifndef INCLUDE_COMPONENT_H
#define INCLUDE_COMPONENT_H

#include "ForwardDecs.h"

class Component
{
	public:
        GameObject* gameObject;
        bool visible = true;
        bool active = true; // TODO - make work

        virtual ~Component() = default;
        virtual void update(float deltaTime);
		virtual void draw();
        virtual void setGameObject(GameObject* go);

		virtual void debugDraw();
		virtual void onCollisionEnter(GameObject* other);
        /* TODO - Use these instead of existing

        order is:
            while(fixedTime < time) {
                FixedUpdate(); (threaded)
                FixedUpdateApply();
                CollisionXXX(other);
            }
            Update(dt);
            Create();
            Activate();
            Deactivate();
            Destroy();
            DrawSolid();
            DrawTransparent();
            DrawDebug();
            FrameEnd();

        virtual ~Component() = default;
        virtual void Update(float deltaTime);
        virtual void FixedUpdate();
        virtual void FixedUpdateApply();
        virtual void FrameEnd();
		virtual void DrawSolid();
        virtual void DrawTransparent();
		virtual void DrawDebug();
		virtual void CollisionEnter(GameObject* other);
        virtual void CollisionExit(GameObject* other);
        virtual void CollisionStay(GameObject* other);
        virtual void Create(); // (called on first update after fully constructed)
        virtual void Destroy(); // (called before any components, children, parents, or siblings are actually destroyed)
        virtual void Activate();
        virtual void Deactivate();
        */
};

#endif