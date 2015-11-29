#ifndef INCLUDE_GPU_EMITTER_H
#define INCLUDE_GPU_EMITTER_H

#include "ForwardDecs.h"
#include "Component.h"
#include "Texture.h"
#include "Shader.h"

class GPUEmitter : public Component
{
private:
	GLuint vao, elapsedTimeUniform;
	glm::vec3 prevPosition, velocity;
	Texture* texture;
	Shader* shader;
	float* startPositions; // vec3
	float* startVelocities; // vec3
	float* accelerations; // vec3
	float* startColors, *endColors; // vec3
	float* startOpacities, *endOpacities;
	float* startSizes, *endSizes;
	float* startTimes;
	int* quadCorners;

	GLuint genParticles();

public:
	float minStartSize, maxStartSize, minEndSize, maxEndSize;
	float startOpacity, endOpacity;
	glm::vec3 startColor, endColor;
	glm::vec3 minStartVelocity, maxStartVelocity;
	glm::vec3 minAcceleration, maxAcceleration;
	float minDuration, maxDuration;
	bool burst, trigger;
	int count; // Per second
	bool enabled;

	GPUEmitter(GameObject*, std::string);
	~GPUEmitter();
	void update(float deltaTime);
	void draw();
	void start();
	void restart();
	void setGameObject(GameObject* go);
};

#endif