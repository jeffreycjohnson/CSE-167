#pragma once
#include "ForwardDecs.h"
#include "Component.h"
#include <vector>

class ParticleTrail :
	public Component
{
public:
	ParticleTrail();
	~ParticleTrail();

	void update(float deltaTime) override;
	void draw() override;

	Material * material;
private:
	float waitTime = 0; //quick variable to prevent drawing during initial boid movements
	GLuint vaoHandle;
	GLuint meshBuffer;
	float maxLength;
	int maxPoints;
	void addPoint(glm::vec3 point);
	void uploadData(std::vector<float> &megaArray);
	std::vector<float> pointList;
};

