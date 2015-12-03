#include "Swarm.h"
#include "Renderer.h"
#include "Timer.h"
#include "ObjectLoader.h"
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
		neighbors[i]->transform.position.x = ((float)rand() / RAND_MAX) * 10 - 5;
		neighbors[i]->transform.position.y = ((float) rand() / RAND_MAX) * 10 - 5;
		neighbors[i]->transform.position.z = ((float)rand() / RAND_MAX) * 10 - 5;
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

		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 norm = glm::normalize(neighborVelocities[i]);
		neighbors[i]->transform.setRotate(glm::quat_cast(glm::orientation(norm, up)));
		neighbors[i]->transform.rotate(glm::angleAxis(atanf(1)*-2.f, glm::vec3(0, 1, 0)));
		neighbors[i]->transform.rotate(glm::angleAxis(atanf(1)*4.f, glm::vec3(0, 0, 1)));

		// Call GameObject's update after so they can override changes
		neighbors[i]->update(deltaTime);
	}

	target.x = sin(Timer::time() / 2) * 20;
	target.z = cos(Timer::time() / 2) * 20;
}

void Swarm::draw()
{
	for (int i = 0; i < neighbors.size(); i++)
	{
		// Look at velocity
		/*
		glm::quat prevRotation = neighbors[i]->transform.rotation;
		glm::vec3 up = glm::vec3(0, 1, 0);
		neighbors[i]->transform.rotation = glm::quat();
		glm::vec3 tmp;
		glm::vec3 norm = glm::normalize(neighborVelocities[i]);
		float angle = glm::acos(glm::dot(norm, up));
		tmp = glm::cross(norm, up);
		tmp = glm::cross(tmp, up);
		if (glm::dot(tmp, norm) > 0)
			angle = -angle;

		glm::vec3 cross = glm::normalize(glm::cross(norm, up));
		//neighbors[i]->transform.rotate(glm::angleAxis(angle, cross));
		//neighbors[i]->transform.rotate(prevRotation);
		*/

		neighbors[i]->draw();
		//neighbors[i]->transform.rotation = prevRotation;
	}
	glm::vec3 prevPosition = neighbors[0]->transform.position;
	neighbors[0]->transform.setPosition(target);
	neighbors[0]->draw();
	neighbors[0]->transform.setPosition(prevPosition);

	for (int i = 0; i < obstacles.size(); i++)
	{
		sphere->transform.setPosition(obstacles[i]->position);
		sphere->transform.scaleFactor = { obstacles[i]->radius, obstacles[i]->radius, obstacles[i]->radius };
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
	glm::vec3 tmp = (target - neighbors[current]->transform.position);
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

		d = glm::length(neighbors[current]->transform.position - obstacles[i]->position);
		d = (d - obstacles[i]->radius);
		if (d > 0 && d < AVOID_DISTANCE + neighbors.size() / 4.0f)
		{
			mean += glm::normalize(neighbors[current]->transform.position - obstacles[i]->position) / (d / ((AVOID_DISTANCE + neighbors.size() / 4.0f)));
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