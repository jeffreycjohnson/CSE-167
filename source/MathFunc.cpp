#include "MathFunc.h"

glm::vec3 MathFunc::Lerp(glm::vec3 a, glm::vec3 b, float time)
{
	if (time > 1)
		return b;
	else if (time < 0)
		return a;

	glm::vec3 cur;
	cur.x = a.x * (1 - time) + b.x * time;
	cur.y = a.y * (1 - time) + b.y * time;
	cur.z = a.z * (1 - time) + b.z * time;
	return cur;
}

float MathFunc::Lerp(float a, float b, float time)
{
	if (time > 1)
		return b;
	else if (time < 0)
		return a;

	float cur;
	cur = a * (1 - time) + b * time;
	return cur;
}