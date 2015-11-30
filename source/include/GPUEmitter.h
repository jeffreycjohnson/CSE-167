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
	GLuint vao, emitterPosUniform, emitterVelocityUniform, elapsedTimeUniform, startTimeUniform, 
		minVelocityUniform, maxVelocityUniform, minAccelUniform, maxAccelUniform, minStartSizeUniform, 
		maxStartSizeUniform, minEndSizeUniform, maxEndSizeUniform, burstSeedUniform;
	glm::vec3 prevPosition, velocity;
	Texture* texture;
	Shader* shader;
	float* startTimes;
	float* durations;
	int* quadCorners;
	unsigned int* seeds;
	bool burst, trigger;

	GLuint genParticles();

public:
	float minStartSize, maxStartSize, minEndSize, maxEndSize;
	float startOpacity, endOpacity;
	glm::vec3 startColor, endColor;
	glm::vec3 minStartVelocity, maxStartVelocity;
	glm::vec3 minAcceleration, maxAcceleration;
	float minDuration, maxDuration;
	int count; // Per second
	bool enabled;

	GPUEmitter(GameObject*, std::string, bool);
	~GPUEmitter();
	void update(float deltaTime);
	void draw();
	void init();
	void restart();
	void play();
	void setGameObject(GameObject* go);
};

#endif