#include "Transform.h"
#include <gtc/matrix_transform.hpp>

/**
* Translate
* -Transform Dirty
*/
void Transform::translate(float x, float y, float z) {
    transformMatrixDirty = true;

    position += glm::vec3(x, y, z);
}

void Transform::Transform::translate(glm::vec3& diff) {
    transformMatrixDirty = true;

    position += diff;
}


/**
* Rotate
* -Transform Dirty
* -Normals Dirty
*/
void Transform::rotate(glm::quat& diff) {
    transformMatrixDirty = true;

    rotation *= diff;
}

/**
* Scale
*/
void Transform::scale(float s) {
    transformMatrixDirty = true;

    scaleFactor *= s;
}


/**
* Get Transform Matrix
* -uses parent's matrix as well
*/
glm::mat4 Transform::getTransformMatrix() {
    if (transformMatrixDirty) {
        transformMatrix = glm::mat4(1.0f);
        transformMatrix = glm::translate(transformMatrix, position);
        transformMatrix *= glm::mat4_cast(rotation);
        transformMatrix = glm::scale(transformMatrix, scaleFactor);
        transformMatrixDirty = false;
    }
    glm::mat4 parMat = (parent) ? parent->getTransformMatrix() : glm::mat4(1.f);
    return  parMat * transformMatrix;
}