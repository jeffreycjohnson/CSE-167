#include "BoxCollider.h"
#include "GameObject.h"
#include "Input.h"
#include "Renderer.h"

std::vector<BoxCollider*> BoxCollider::colliders;

BoxCollider::BoxCollider(glm::vec3 offset, glm::vec3 dimensions)
{
	float halfW = dimensions.x / 2;
	float halfH = dimensions.y / 2;
	float halfD = dimensions.z / 2;
	points[0] = offset + glm::vec3(halfW, halfH, halfD);
	points[1] = offset + glm::vec3(halfW, halfH, -halfD);
	points[2] = offset + glm::vec3(halfW, -halfH, halfD);
	points[3] = offset + glm::vec3(halfW, -halfH, -halfD);
	points[4] = offset + glm::vec3(-halfW, halfH, halfD);
	points[5] = offset + glm::vec3(-halfW, halfH, -halfD);
	points[6] = offset + glm::vec3(-halfW, -halfH, halfD);
	points[7] = offset + glm::vec3(-halfW, -halfH, -halfD);
	colliders.push_back(this);
	colliding = false;
	passive = true;
}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::update(float deltaTime)
{
	glm::mat4 matrix = gameObject->transform.getTransformMatrix();
	for (int i = 0; i < 8; i++)
	{
		transformPoints[i] = glm::vec3(matrix * glm::vec4(points[i], 1));
	}

	// Calculate axis aligned bounding box
	xmin = xmax = transformPoints[0].x;
	ymin = ymax = transformPoints[0].y;
	zmin = zmax = transformPoints[0].z;
	for (int i = 1; i < 8; i++)
	{
		if (xmin > transformPoints[i].x)
			xmin = transformPoints[i].x;
		if (xmax < transformPoints[i].x)
			xmax = transformPoints[i].x;

		if (ymin > transformPoints[i].y)
			ymin = transformPoints[i].y;
		if (ymax < transformPoints[i].y)
			ymax = transformPoints[i].y;

		if (zmin > transformPoints[i].z)
			zmin = transformPoints[i].z;
		if (zmax < transformPoints[i].z)
			zmax = transformPoints[i].z;
	}
}

void BoxCollider::draw()
{
}

void BoxCollider::debugDraw()
{
	drawDebugCube(transformPoints);
	glm::vec3 aabb[8];
	aabb[0] = glm::vec3(xmax, ymax, zmax);
	aabb[1] = glm::vec3(xmax, ymax, zmin);
	aabb[2] = glm::vec3(xmax, ymin, zmax);
	aabb[3] = glm::vec3(xmax, ymin, zmin);
	aabb[4] = glm::vec3(xmin, ymax, zmax);
	aabb[5] = glm::vec3(xmin, ymax, zmin);
	aabb[6] = glm::vec3(xmin, ymin, zmax);
	aabb[7] = glm::vec3(xmin, ymin, zmin);
	drawDebugCube(aabb);
	colliding = false;
}

void BoxCollider::setGameObject(GameObject* go)
{
	gameObject = go;
}

void BoxCollider::onCollisionEnter(GameObject* other)
{
	if (!passive)
		colliding = true;
}

void BoxCollider::drawDebugCube(glm::vec3 list[])
{
	if (colliding)
		glColor3f(1, 0, 0);
	else
		glColor3f(0, 1, 0);

	Renderer::setModelMatrix(glm::mat4());
	glBegin(GL_LINES);
	
	glVertex3f(list[0].x, list[0].y, list[0].z);
	glVertex3f(list[1].x, list[1].y, list[1].z);
	glVertex3f(list[0].x, list[0].y, list[0].z);
	glVertex3f(list[2].x, list[2].y, list[2].z);
	glVertex3f(list[0].x, list[0].y, list[0].z);
	glVertex3f(list[4].x, list[4].y, list[4].z);
	
	glVertex3f(list[5].x, list[5].y, list[5].z);
	glVertex3f(list[1].x, list[1].y, list[1].z);
	glVertex3f(list[5].x, list[5].y, list[5].z);
	glVertex3f(list[4].x, list[4].y, list[4].z);
	glVertex3f(list[5].x, list[5].y, list[5].z);
	glVertex3f(list[7].x, list[7].y, list[7].z);
	
	glVertex3f(list[6].x, list[6].y, list[6].z);
	glVertex3f(list[2].x, list[2].y, list[2].z);
	glVertex3f(list[6].x, list[6].y, list[6].z);
	glVertex3f(list[7].x, list[7].y, list[7].z);
	glVertex3f(list[6].x, list[6].y, list[6].z);
	glVertex3f(list[4].x, list[4].y, list[4].z);
	
	glVertex3f(list[3].x, list[3].y, list[3].z);
	glVertex3f(list[1].x, list[1].y, list[1].z);
	glVertex3f(list[3].x, list[3].y, list[3].z);
	glVertex3f(list[2].x, list[2].y, list[2].z);
	glVertex3f(list[3].x, list[3].y, list[3].z);
	glVertex3f(list[7].x, list[7].y, list[7].z);
	
	glEnd();
	
}

void BoxCollider::remove()
{
	for (int i = 0; i < colliders.size(); i++)
	{
		if (colliders[i] == this)
			colliders.erase(colliders.begin() + i);
	}
}

void BoxCollider::updateColliders()
{
	// Optimize with sweep and prune eventually, for now use brute force
	for (int i = 0; i < colliders.size(); i++)
	{
		// Optimize erasing colliders? How often will this really happen?
		while (i < colliders.size() && (colliders[i] == nullptr || colliders[i]->gameObject == nullptr))
		{
			colliders.erase(colliders.begin() + i);
		}
		if (i < colliders.size() && !colliders[i]->passive)
		{
			for (int e = 0; e < colliders.size(); e++)
			{
				while (e < colliders.size() && (colliders[e] == nullptr || colliders[e]->gameObject == nullptr))
				{
					if (i > e)
						i--;

					colliders.erase(colliders.begin() + e);
				}
				if (e < colliders.size() && i != e && checkCollision(i, e))
				{
					// Check for precise collision?
					colliders[i]->gameObject->onCollisionEnter(colliders[e]->gameObject);
					colliders[e]->gameObject->onCollisionEnter(colliders[i]->gameObject);
				}
			}
		}
	}
}

bool BoxCollider::checkCollision(int aIndex, int bIndex)
{
	BoxCollider* a = colliders[aIndex];
	BoxCollider* b = colliders[bIndex];

	if (a != nullptr && b != nullptr)
	{
		bool collideX = false, collideY = false, collideZ = false;

		if (a->xmin <= b->xmax && b->xmin <= a->xmax)
			collideX = true;
		if (a->ymin <= b->ymax && b->ymin <= a->ymax)
			collideY = true;
		if (a->zmin <= b->zmax && b->zmin <= a->zmax)
			collideZ = true;

		if (collideX && collideY && collideZ)
			return true;
	}
	return false;
}