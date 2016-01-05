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
		maxStartSizeUniform, minEndSizeUniform, maxEndSizeUniform, burstSeedUniform, 
		minStartColorUniform, maxStartColorUniform, minEndColorUniform, maxEndColorUniform, 
		minStartAngleUniform, maxStartAngleUniform, minAngularVelocityUniform, maxAngularVelocityUniform,
		startOpacityUniform, endOpacityUniform, rotateTowardsVelocityUniform;
	glm::vec3 prevPosition, velocity;
	Texture* texture;
	Shader* shader;
	float* startTimes; // For standard emitter
	float* durations;
	int* quadCorners;
	unsigned int* seeds;
	float startTime, burstSeed; // For burst
	glm::vec3 burstStartPos;
	bool burst, trigger;

	GLuint genParticles();

public:
	float minStartSize, maxStartSize, minEndSize, maxEndSize;
	float startOpacity, endOpacity;
	glm::vec3 minStartColor, maxStartColor, minEndColor, maxEndColor;
	glm::vec3 minStartVelocity, maxStartVelocity, emitterVelocity;
	glm::vec3 minAcceleration, maxAcceleration;
	float minDuration, maxDuration;
	float minStartAngle, maxStartAngle, minAngularVelocity, maxAngularVelocity;
	float emitterVelocityScale;
	int count; // Per second
	bool enabled, loop, additive, rotateTowardsVelocity;

	GPUEmitter(GameObject*, std::string, bool);
	~GPUEmitter();
	void update(float deltaTime) override;
	void draw() override;
	void init();
	void play();
};

#endif