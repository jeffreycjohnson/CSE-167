#include "Swarm.h"
#include "Renderer.h"
#include "Timer.h"
#include "ObjectLoader.h"
#include "MathFunc.h"
#include <random>
#include <time.h>
#include <gtc/quaternion.hpp>
#include <gtx/rotate_vector.hpp>

unsigned int Swarm::currentID = 1; // Start at 1, ignore ID's of 0
std::vector<BoidSphere*> Swarm::obstacles;
GameObject* sphere;
Material* mat;
Texture* norm;

Swarm::Swarm(GameObject** models, int count)
{
	id = -1;
	glm::vec3 tmp;
	for (int i = 0; i < count; i++)
	{
		neighbors.push_back(models[i]);
		neighborVelocities.push_back(tmp);
	}
	sphere = loadScene("assets/test_sphere.obj");
	mat = new Material(Renderer::getShader(FORWARD_PBR_SHADER));
	norm = new Texture("assets/test_sphere_normal.png", false);
	(*mat)["useTextures"] = false;
	(*mat)["testMetal"] = 0.5f;
	(*mat)["testRough"] = 0.5f;
	(*mat)["normalTex"] = norm;
	sphere->setMaterial(mat);
	init();
}

Swarm::~Swarm()
{
	delete sphere;
	delete mat;
	delete norm;
}

void Swarm::init()
{
	srand(time(NULL));
	for (int i = 0; i < neighbors.size(); i++)
	{
		neighbors[i]->transform.position.x += ((float)rand() / RAND_MAX) * 10 - 5;
		neighbors[i]->transform.position.y += ((float) rand() / RAND_MAX) * 10 - 5;
		neighbors[i]->transform.position.z += ((float)rand() / RAND_MAX) * 10 - 5;
	}
}

void Swarm::update(float deltaTime)
{
	averagePosition = {0, 0, 0};
	for (int i = 0; i < neighbors.size(); i++)
	{
		averagePosition += neighbors[i]->transform.position;
	}
	averagePosition /= neighbors.size();

	for (int i = 0; i < neighborVelocities.size(); i++)
	{
		neighborVelocities[i] += cohere(i) + align(i) + separate(i)
			+ travel(i) + avoidObstacles(i);
		neighborVelocities[i] *= 3.0f;
		neighborVelocities[i] = limitSpeed(neighborVelocities[i], MAX_SPEED);
	}

	for (int i = 0; i < neighbors.size(); i++)
	{
		neighbors[i]->transform.translate((float) Timer::deltaTime() * neighborVelocities[i]);

		if (neighborVelocities[i].length() >= 1e-6f) {
			glm::vec3 norm = glm::normalize(neighborVelocities[i]);
			float theta = atan2f(norm.x, norm.z);
			float planeDist = sqrtf(norm.x * norm.x + norm.z * norm.z);
			float phi = -atan2f(norm.y, planeDist);
			neighbors[i]->transform.setRotate(glm::angleAxis(theta, glm::vec3(0, 1, 0)));
			neighbors[i]->transform.rotate(glm::angleAxis(phi, glm::vec3(1, 0, 0)));
		}

		// Call GameObject's update after so they can override changes
		neighbors[i]->update(deltaTime);
	}

	float distance = glm::length(target - currentTarget);
	if (abs(distance) < 0.001)
	{
		prevTarget = target;
		glm::vec3 direction(((float)rand() / RAND_MAX - 0.5), ((float)rand() / RAND_MAX - 0.5), ((float)rand() / RAND_MAX - 0.5));
		direction = glm::normalize(direction);
		direction *= MAX_SPEED * 6; // Set distance
		direction += target; // Offset from current location

		if (direction.x > BOUNDS)
			direction.x = BOUNDS;
		else if (direction.x < -BOUNDS)
			direction.x = -BOUNDS;

		if (direction.y > BOUNDS)
			direction.y = BOUNDS;
		else if (direction.y < -BOUNDS)
			direction.y = -BOUNDS;

		if (direction.z > BOUNDS)
			direction.z = BOUNDS;
		else if (direction.z < -BOUNDS)
			direction.z = -BOUNDS;

		target = direction;

		startTime = waitTime = Timer::time();
		waitAmount = glm::length(target - prevTarget);
	}
	else
	{
		float targetDistance = glm::length(target - prevTarget);
		currentTarget = MathFunc::Lerp(prevTarget, target, (Timer::time() - startTime) / (targetDistance / MAX_SPEED / 2));
	}
}

void Swarm::draw()
{
	
}

void Swarm::debugDraw()
{
	sphere->transform.setPosition(currentTarget);
	sphere->transform.scaleFactor = glm::vec3(1, 1, 1);
	sphere->draw();

	for (int i = 0; i < obstacles.size(); i++)
	{
		sphere->transform.setPosition(obstacles[i]->transform->getWorldPosition());
		sphere->transform.scaleFactor = obstacles[i]->transform->getWorldScale() * glm::vec3(1, 1, 1);
		sphere->draw();
	}
}

glm::vec3 Swarm::cohere(int current)
{
	glm::vec3 mean;

	for (int i = 0; i < neighbors.size(); i++)
	{
		if (i == current) // Skip myself
			continue;

		mean += neighbors[i]->transform.position;
	}
	mean /= neighbors.size() - 1;

	// Steer towards average location
	glm::vec3 desired = mean - neighbors[current]->transform.position;
	float d = glm::length(desired);

	if (d > 0)
	{
		desired = glm::normalize(desired);

		if (d < DAMPING)
			desired *= MAX_COHERE_SPEED * d / DAMPING;
		else
			desired *= MAX_COHERE_SPEED;

		mean = desired - neighborVelocities[current];
	}
	else
	{
		mean = { 0, 0, 0 };
	}
	return mean;
}

glm::vec3 Swarm::align(int current)
{
	glm::vec3 mean;

	for (int i = 0; i < neighborVelocities.size(); i++)
	{
		if (i == current) // Skip myself
			continue;

		mean += neighborVelocities[i];
	}
	mean /= neighborVelocities.size() - 1;
	return mean / 30.0f;
}

glm::vec3 Swarm::separate(int current)
{
	glm::vec3 mean;
	float d;

	for (int i = 0; i < neighbors.size(); i++)
	{
		if (i == current) // Skip myself
			continue;

		d = glm::length(neighbors[current]->transform.position - neighbors[i]->transform.position);
		if (d > 0 && d < SEPARATION_DISTANCE + neighbors.size() / 4.0f)
			mean += glm::normalize(neighbors[current]->transform.position - neighbors[i]->transform.position) / (d / (SEPARATION_DISTANCE + neighbors.size() / 2.0f));
	}
	mean /= neighbors.size() - 1;
	mean = limitSpeed(mean, MAX_SPEED);
	return mean;
}

glm::vec3 Swarm::travel(int current)
{
	glm::vec3 tmp = (currentTarget - neighbors[current]->transform.position);
	tmp /= 5;
	tmp = limitSpeed(tmp, MAX_SPEED);
	return  tmp;
}

glm::vec3 Swarm::avoidObstacles(int current)
{
	glm::vec3 mean;
	float d;
	int hasSelf = 0;

	for (int i = 0; i < obstacles.size(); i++)
	{
		if (i == id) // Skip myself
		{
			hasSelf = 1;
			continue;
		}

		glm::vec3 currentObstaclePosition = obstacles[i]->transform->getWorldPosition();
		float currentObstacleRadius = obstacles[i]->transform->getWorldScale();

		d = glm::length(neighbors[current]->transform.position - currentObstaclePosition);
		d = (d - currentObstacleRadius);
		if (d > 0 && d < AVOID_DISTANCE + neighbors.size() / 4.0f)
		{
			mean += glm::normalize(neighbors[current]->transform.position - currentObstaclePosition) / (d / ((AVOID_DISTANCE + neighbors.size() / 4.0f)));
		}
	}
	mean /= obstacles.size() - hasSelf;
	mean = limitSpeed(mean, MAX_SPEED);
	return mean;
}

glm::vec3 Swarm::limitSpeed(glm::vec3 current, float maxSpeed)
{
	float max = glm::max(current.x, current.y);
	max = glm::max(max, current.z);

	if (abs(max) > maxSpeed)
	{
		current /= abs(max) / maxSpeed;
	}

	return current;
}

void Swarm::addObstacle(BoidSphere* newObstacle)
{
	newObstacle->id = currentID;
	obstacles.push_back(newObstacle);
	currentID++;
}