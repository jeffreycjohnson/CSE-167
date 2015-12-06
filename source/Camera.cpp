#include "Camera.h"
#include "Timer.h"
#include "Input.h"
#include "MathFunc.h"
#include "Renderer.h"
#include <gtc/matrix_inverse.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm.hpp>
#include <random>
#include <time.h>

using namespace std;

Camera::Camera()
{
	currentFOV = fov = atan(1) * 4.0f / 3.0f;
	fovDuration = 1;
	offset.setPosition(0, 0, 0);
	up = {0, 1, 0};
	srand(time(NULL));
}

Camera::~Camera()
{

}

glm::mat4 Camera::getCameraMatrix()
{
	//glm::affineInverse seems to have a bug with inverting scales atm (fixed in git, but not in lastest stable release), so we need to remove the scale manually
	
	matrix = gameObject->transform.getTransformMatrix() * offset.getTransformMatrix();
	glm::vec3 scale;
	scale[0] = 1/glm::length(glm::vec3(matrix[0][0], matrix[1][0], matrix[2][0]));
	scale[1] = 1/glm::length(glm::vec3(matrix[0][1], matrix[1][1], matrix[2][1]));
	scale[2] = 1/glm::length(glm::vec3(matrix[0][2], matrix[1][2], matrix[2][2]));
	return glm::affineInverse(glm::scale(matrix, scale));
}

void Camera::update(float deltaTime)
{
	if (fov != prevFOV)
	{
		fovStartTime = Timer::time();
		prevFOV = fov;
	}

	currentFOV = MathFunc::Lerp(currentFOV, fov, (float) (Timer::time() - fovStartTime) / fovDuration);

	if (isShaking)
	{
		float x = (float)rand() / RAND_MAX * shakeAmount - shakeAmount / 2.0f;
		float y = (float)rand() / RAND_MAX * shakeAmount - shakeAmount / 2.0f;
		float z = (float)rand() / RAND_MAX * shakeAmount - shakeAmount / 2.0f;
		offset.setPosition(x, y, z);

		shakeAmount = startShakeAmount * shakeDuration / startShakeDuration;
		shakeDuration -= deltaTime;

		if (shakeDuration <= 0)
		{
			shakeDuration = 0;
			shakeAmount = 0;
			isShaking = false;
			offset.setPosition(0, 0, 0);
		}
	}

	forward = { Renderer::view[0][2], Renderer::view[1][2], Renderer::view[2][2] };
	position = { matrix[3][0], matrix[3][1], matrix[3][2] }; // Has to be world space to work with forward vector
	velocity = position - prevPosition;
	prevPosition = position;

	// Update info for FMOD
	FMOD_VECTOR pos = { position.x, position.y, position.z };
	FMOD_VECTOR vel = { velocity.x, velocity.y, velocity.z };
	FMOD_VECTOR fwd = { forward.x, forward.y, forward.z };
	FMOD_VECTOR upv = { up.x, up.y, up.z };
	Sound::system->set3DListenerAttributes(0, &pos, &vel, &fwd, &upv);
}

void Camera::screenShake(float amount, float duration)
{
	shakeAmount = startShakeAmount = amount;
	shakeDuration = startShakeDuration = duration;
	isShaking = true;
}

glm::vec3 Camera::getForward()
{
	return -forward;
}

glm::vec3 Camera::getVelocity()
{
	return velocity;
}

float Camera::getFOV()
{
	return currentFOV;
}