#ifndef INCLUDE_CAMERA_H
#define INCLUDE_CAMERA_H

#include "ForwardDecs.h"
#include "GameObject.h"
#include "Transform.h"

class Camera : public GameObject
{
	private:
		float shakeAmount, startShakeAmount, shakeDuration, startShakeDuration;
		bool isShaking;
		float currentFOV, prevFOV, fovStartTime;

	public:
		Transform offset;
		float fov, fovDuration;

		Camera();
		~Camera();
		glm::mat4 getCameraMatrix();
		void update(float deltaTime);
		void screenShake(float amount, float duration);
		float getFOV();
};

#endif