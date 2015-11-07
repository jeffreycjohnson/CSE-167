#ifndef INCLUDE_CAMERA_H
#define INCLUDE_CAMERA_H

#include "ForwardDecs.h"
#include "GameObject.h"

class Camera : public GameObject
{
	public:
		glm::mat4 getCameraMatrix();
};

#endif