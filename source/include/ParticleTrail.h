#pragma once
#include "ForwardDecs.h"
#include "Component.h"
#include <deque>

class ParticleTrail :
	public Component
{
public:
	ParticleTrail();
	~ParticleTrail();

	void update(float deltaTime) override;
	void draw() override;

	bool additive = true;
	Material * material;
private:
	struct TrailPoint {
		glm::vec3 position;
		glm::vec3 normal;
		//added before shader upload //
		/* float distance - distance from front of trail */
		/* float corner */
	};
	const int elementStride = 3 + 3 + 1 + 1; //number of floats per point

	GLuint vaoHandle;
	GLuint meshBuffer;
	
	const int maxPoints = 64;
	float* megaArray;

	const float addDelayTime = 0.32f;//in seconds
	float currentDelayTime = 0;

	void addPoint(glm::vec3 point);
	void uploadData();
	std::deque<TrailPoint> pointList;
};

