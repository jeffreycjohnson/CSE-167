#include "Swarm.h"
#include "Renderer.h"
#include "Timer.h"
#include "ObjectLoader.h"
#include <random>

unsigned int Swarm::currentID = 1; // Start at 1, ignore ID's of 0
std::vector<BoidSphere*> Swarm::obstacles;

Swarm::Swarm(GameObject* model, int count)
{
	glm::vec3 tmp;
	for (int i = 0; i < count; i++)
	{
		// Move to outside
		GameObject* go = loadScene("assets/test_sphere.obj");
		Texture* blankNormal = new Texture("assets/blank_normal.png");
		Material* sphereMat = new Material(Renderer::getShader(FORWARD_PBR_SHADER));
		(*sphereMat)["useTextures"] = false;
		(*sphereMat)["testMetal"] = 0.5f;
		(*sphereMat)["testRough"] = 0.5f;
		(*sphereMat)["normalTex"] = blankNormal;
		go->setMaterial(sphereMat);

		neighbors.push_back(go);
		neighborVelocities.push_back(tmp);
	}
	init();
}

Swarm::~Swarm()
{

}

void Swarm::init()
{
	srand(glfwGetTime());
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
		glm::vec3 tmp = neighborVelocities[i];
		tmp *= Timer::deltaTime();
		neighbors[i]->transform.translate(tmp);

		// Call GameObject's update after so they can override changes
		neighbors[i]->update(deltaTime);
	}

	target.x = sin(Timer::time() / 2) * 50;
}

void Swarm::draw()
{
	for (int i = 0; i < neighbors.size(); i++)
	{
		neighbors[i]->draw();
	}
	glm::vec3 prev = neighbors[0]->transform.position;
	neighbors[0]->transform.setPosition(target);
	neighbors[0]->draw();
	neighbors[0]->transform.setPosition(prev);

	for (int i = 0; i < obstacles.size(); i++)
	{
		glm::vec3 prev = neighbors[0]->transform.position;
		glm::vec3 prevScale = neighbors[0]->transform.scaleFactor;
		neighbors[0]->transform.setPosition(obstacles[i]->position);
		neighbors[0]->transform.scaleFactor = { obstacles[i]->radius, obstacles[i]->radius, obstacles[i]->radius };
		neighbors[0]->draw();
		neighbors[0]->transform.setPosition(prev);
		neighbors[0]->transform.scaleFactor = prevScale;
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