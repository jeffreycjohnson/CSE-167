#ifndef INCLUDE_SWARM_H
#define INCLUDE_SWARM_H

#include "ForwardDecs.h"
#include "GameObject.h"
#include "Transform.h"
#include <vector>

#define MAX_COHERE_SPEED 100.0f
#define MAX_SPEED 40.0f
#define DAMPING 200.0f
#define SEPARATION_DISTANCE 80.0f
#define AVOID_DISTANCE 200.0f

struct BoidSphere : public GameObject
{
	unsigned int id;
	glm::vec3 position;
	float radius;
};

class Swarm
{
private:
	static unsigned int currentID;
	static std::vector<BoidSphere*> obstacles;

	int id;
	glm::vec3 averagePosition;
	std::vector<GameObject*> neighbors;
	std::vector<glm::vec3> neighborVelocities;

	glm::vec3 cohere(int index);
	glm::vec3 align(int index);
	glm::vec3 separate(int index);
	glm::vec3 travel(int index);
	glm::vec3 avoidObstacles(int index);
	glm::vec3 limitSpeed(glm::vec3 index, float maxSpeed);

public:
	glm::vec3 target;
	float collisionRadius, neighborRadius;

	Swarm(GameObject** models, int count);
	~Swarm();

	void init();

	void update(float deltaTime);
	void draw();

	static void addObstacle(BoidSphere* newObstacle);
};

#endif