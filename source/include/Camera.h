#ifndef INCLUDE_CAMERA_H
#define INCLUDE_CAMERA_H

#include "ForwardDecs.h"
#include "GameObject.h"
#include "Transform.h"
#include "Sound.h" // For FMOD system

class Camera : public Component
{
	private:
		float shakeAmount, startShakeAmount, shakeDuration, startShakeDuration;
		bool isShaking;
		float currentFOV, prevFOV, fovStartTime;
		glm::vec3 forward, up, position, prevPosition, velocity;
		glm::mat4 matrix;

	public:
		Transform offset;
		float fov, fovDuration;

		Camera();
		~Camera();
		glm::mat4 getCameraMatrix();
		void update(float deltaTime);
		void screenShake(float amount, float duration);
		glm::vec3 getForward();
		glm::vec3 getVelocity();
		float getFOV();
};

#endif