#include "Camera.h"
#include "Timer.h"
#include "Input.h"
#include "MathFunc.h"
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
	transform.setPosition(0, 0, 0);
	srand(time(NULL));
}

Camera::~Camera()
{

}

glm::mat4 Camera::getCameraMatrix()
{
	//glm::affineInverse seems to have a bug with inverting scales atm (fixed in git, but not in lastest stable release), so we need to remove the scale manually
	
	glm::mat4 matrix = transform.getTransformMatrix() * offset.getTransformMatrix();
	glm::vec3 scale;
	scale[0] = 1/glm::length(glm::vec3(matrix[0][0], matrix[1][0], matrix[2][0]));
	scale[1] = 1/glm::length(glm::vec3(matrix[0][1], matrix[1][1], matrix[2][1]));
	scale[2] = 1/glm::length(glm::vec3(matrix[0][2], matrix[1][2], matrix[2][2]));
	return glm::affineInverse(glm::scale(matrix, scale));
}

void Camera::update(float deltaTime)
{
	if (Input::getKeyDown("space"))
		screenShake(0.5, 1);
	if (Input::getKeyDown("down"))
		fov += 0.75;
	if (Input::getKeyDown("up"))
		fov -= 0.75;

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
}

void Camera::screenShake(float amount, float duration)
{
	shakeAmount = startShakeAmount = amount;
	shakeDuration = startShakeDuration = duration;
	isShaking = true;
}

float Camera::getFOV()
{
	return currentFOV;
}