#ifndef INCLUDE_CAMERA_H
#define INCLUDE_CAMERA_H

#include "ForwardDecs.h"
#include "GameObject.h"

class Camera : GameObject
{
	public:
		glm::mat4 getCameraMatrix();
};

#endif