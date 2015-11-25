#ifndef INCLUDE_MATH_H
#define INCLUDE_MATH_H

#include "ForwardDecs.h"

class MathFunc
{
public:
	static glm::vec3 Lerp(glm::vec3, glm::vec3, float);
	static float Lerp(float, float, float);
};

#endif