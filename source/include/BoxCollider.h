#ifndef INCLUDE_BOX_COLLIDER_H
#define INCLUDE_BOX_COLLIDER_H

#include "Component.h"
#include <vector>

class BoxCollider : public Component
{
private:
	static std::vector<BoxCollider*> colliders;
	glm::vec3 points[8];
	glm::vec3 transformPoints[8];
	float xmin, xmax, ymin, ymax, zmin, zmax;
	bool colliding;

public:
	bool passive;

	BoxCollider(glm::vec3 offset, glm::vec3 dimensions);
	~BoxCollider();
	void remove();
	void update(float);
	void draw();
	void debugDraw();
	void setGameObject(GameObject* go);
	void onCollisionEnter(GameObject* other);
	void drawDebugCube(glm::vec3 list[]);

	static void updateColliders();
	static bool checkCollision(int a, int b);
};

#endif