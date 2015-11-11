#include "Camera.h"
#include <gtc/matrix_inverse.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm.hpp>

glm::mat4 Camera::getCameraMatrix() {
	//glm::affineInverse seems to have a bug with inverting scales atm (fixed in git, but not in lastest stable release), so we need to remove the scale manually
	
	glm::mat4 matrix = transform.getTransformMatrix();
	glm::vec3 scale;
	scale[0] = 1/glm::length(glm::vec3(matrix[0][0], matrix[1][0], matrix[2][0]));
	scale[1] = 1/glm::length(glm::vec3(matrix[0][1], matrix[1][1], matrix[2][1]));
	scale[2] = 1/glm::length(glm::vec3(matrix[0][2], matrix[1][2], matrix[2][2]));
	return glm::affineInverse(glm::scale(matrix, scale));
}