#ifndef INCLUDE_MATH_H
#define INCLUDE_MATH_H

#include "ForwardDecs.h"

class MathFunc
{
public:
	static glm::vec3 Lerp(glm::vec3 a, glm::vec3 b, float time);
	static float Lerp(float a, float b, float time);
};

#endif