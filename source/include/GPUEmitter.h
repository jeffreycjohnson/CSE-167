#ifndef INCLUDE_GAME_OBJECT_H
#define INCLUDE_GAME_OBJECT_H

#include "Component.h"

class GPUEmitter : public Component
{
private:

public:
	virtual ~GPUEmitter();
	virtual void update(float deltaTime);
	virtual void setGameObject(GameObject* go);
};

#endif