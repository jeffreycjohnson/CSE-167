#include "Camera.h"
#include "gtc/matrix_inverse.hpp"

glm::mat4 Camera::getCameraMatrix() {
	//this seems to have a bug with inverting scales (fixed in git, but not in lastest stable release), but that shouldn't apply to the camera
	return glm::affineInverse(transform.getTransformMatrix());
}