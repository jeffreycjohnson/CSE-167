#ifndef INCLUDE_GPU_EMITTER_H
#define INCLUDE_GPU_EMITTER_H

#include "ForwardDecs.h"
#include "Component.h"
#include "Texture.h"
#include "Shader.h"
#include <random>

class GPUEmitter : public Component
{
private:
	GLuint vao, elapsedTimeUniform, deltaTimeUniform;
	glm::vec3 prevPosition, velocity;
	Texture* texture;
	Shader* shader;
	float* startPositions; // vec3
	float* startVelocities; // vec3
	float* startTimes;
	float* durations;
	int* quadCorners;
	unsigned int* seeds;

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