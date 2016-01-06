#include "Swarm.h"
#include "Renderer.h"
#include "Timer.h"
#include "ObjectLoader.h"
#include <random>
#include <time.h>
#include <gtc/quaternion.hpp>
#include <gtx/compatibility.hpp>

unsigned int Swarm::currentID = 1; // Start at 1, ignore ID's of 0
std::vector<BoidSphere*> Swarm::obstacles;

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
	if(sphere) delete sphere;
	if(mat) delete mat;
	if(norm) delete norm;
}

void Swarm::init()
{
	srand(time(NULL));
	for (auto neighbor : neighbors)
	{
		neighbor->transform.position.x += ((float)rand() / RAND_MAX) * 10 - 5;
		neighbor->transform.position.y += ((float) rand() / RAND_MAX) * 10 - 5;
		neighbor->transform.position.z += ((float)rand() / RAND_MAX) * 10 - 5;
        auto f = neighbor->getComponent<Fighter>();
        if(f)
        {
            f->swarm = this;
        }
	}
}

void Swarm::update(float deltaTime)
{
	averagePosition = {0, 0, 0};
	for (auto neighbor : neighbors)
	{
		averagePosition += neighbor->transform.position;
	}
	averagePosition /= neighbors.size();

	for (unsigned int i = 0; i < neighborVelocities.size(); i++)
	{
		neighborVelocities[i] += cohere(i) + align(i) + separate(i)
			+ travel(i) + avoidObstacles(i);
		neighborVelocities[i] *= 3.0f;
		neighborVelocities[i] = limitSpeed(neighborVelocities[i], MAX_SPEED);
	}

	for (unsigned int i = 0; i < neighbors.size(); i++)
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
	if (fabs(distance) < 0.001)
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
		currentTarget = glm::lerp(prevTarget, target, glm::clamp((float)(Timer::time() - startTime) / (targetDistance / MAX_SPEED / 2), 0.f, 1.f));
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

	for (auto obstacle : obstacles)
	{
		sphere->transform.setPosition(obstacle->transform->getWorldPosition());
		sphere->transform.scaleFactor = obstacle->transform->getWorldScale() * glm::vec3(1, 1, 1);
		sphere->draw();
	}
}

void Swarm::remove(GameObject* fighter)
{
    int i = 0;
    for (auto iter = neighbors.begin(); iter < neighbors.end(); iter++)
    {
        if (*iter == fighter)
        {
            neighbors.erase(iter);
            break;
        }
        i++;
    }
    auto iter = neighborVelocities.begin();
    while(i-- > 0)
    {
        iter++;
    }
    neighborVelocities.erase(iter);
}

glm::vec3 Swarm::cohere(int current)
{
	glm::vec3 mean;

	for (auto neighbor : neighbors)
	{
		mean += neighbor->transform.getWorldPosition();
	}
	mean /= neighbors.size();

	// Steer towards average location
	glm::vec3 desired = mean - neighbors[current]->transform.getWorldPosition();
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

	for (auto velocity : neighborVelocities)
	{
        mean += velocity;
	}
	mean /= neighborVelocities.size();
	return mean / 30.0f;
}

glm::vec3 Swarm::separate(int current)
{
	glm::vec3 mean;
	float d;

	for (auto neighbor : neighbors)
	{
		d = glm::length(neighbors[current]->transform.getWorldPosition() - neighbor->transform.getWorldPosition());
		if (d > 0 && d < SEPARATION_DISTANCE + neighbors.size() / 4.0f)
			mean += glm::normalize(neighbors[current]->transform.getWorldPosition() - neighbor->transform.getWorldPosition()) / (d / (SEPARATION_DISTANCE + neighbors.size() / 2.0f));
	}
	mean /= neighbors.size();
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

	for (unsigned int i = 0; i < obstacles.size(); i++)
	{
		if ((signed)i == id) // Skip myself
		{
			hasSelf = 1;
			continue;
		}

		glm::vec3 currentObstaclePosition = obstacles[i]->transform->getWorldPosition();
		float currentObstacleRadius = obstacles[i]->transform->getWorldScale();

		d = glm::length(neighbors[current]->transform.getWorldPosition() - currentObstaclePosition);
		d = (d - currentObstacleRadius);
		if (d > 0 && d < AVOID_DISTANCE + neighbors.size() / 4.0f)
		{
			mean += glm::normalize(neighbors[current]->transform.getWorldPosition() - currentObstaclePosition) / (d / ((AVOID_DISTANCE + neighbors.size() / 4.0f)));
		}
	}
	mean = limitSpeed(mean, MAX_SPEED);
	return mean;
}

glm::vec3 Swarm::limitSpeed(glm::vec3 current, float maxSpeed)
{
	float max = glm::max(current.x, current.y);
	max = glm::max(max, current.z);

	if (fabs(max) > maxSpeed)
	{
		current /= fabs(max) / maxSpeed;
	}

	return current;
}

void Swarm::addObstacle(BoidSphere* newObstacle)
{
	newObstacle->id = currentID;
	obstacles.push_back(newObstacle);
	currentID++;
}