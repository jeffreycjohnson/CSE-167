#ifndef INCLUDE_TRANSFORM_H
#define INCLUDE_TRANSFORM_H

#include "ForwardDecs.h"
#include "gtc/quaternion.hpp"
#include "gtc/matrix_transform.hpp"
#include <vector>
#include<iostream>

class Transform
{
	public:
		//parent Transform
		Transform* parent = 0;

		//child Transforms
		std::vector<std::shared_ptr<Transform>> children;

		//Position - vector
		glm::vec3 position;
		//Rotation - quaternion
		glm::quat rotation;
		//Scale - vector
		glm::vec3 scaleFactor = glm::vec3(1,1,1);

		//dirty flag for Transform Matrix
		bool transformMatrixDirty = true;
		//Cached Transform Matrix
		glm::mat4 transformMatrix;


		/**
		 * Translate
		 * -Transform Dirty
		 */
		void translate(float x, float y, float z) {
			transformMatrixDirty = true;

			position += glm::vec3(x, y, z);
		}

		void translate(glm::vec3& diff) {
			transformMatrixDirty = true;

			position += diff;
		}


		/**
		 * Rotate
		 * -Transform Dirty
		 * -Normals Dirty
		 */
		void rotate(glm::quat& diff) {
			transformMatrixDirty = true;

			rotation *= diff;
		}

		/**
		 * Scale
		 */
		void scale(float s) {
			transformMatrixDirty = true;

			scaleFactor *= s;
		}


		/**
		 * Get Transform Matrix
		 * -uses parent's matrix as well
		 */
		glm::mat4 getTransformMatrix() {
			if (transformMatrixDirty) {
				transformMatrix = glm::mat4(1.0f);
				transformMatrix = glm::translate(transformMatrix, position);
				transformMatrix *= ((glm::mat4)rotation);
				transformMatrix = glm::scale(transformMatrix, scaleFactor);
				transformMatrixDirty = false;
			}
			glm::mat4 parMat = (parent)? parent->getTransformMatrix() : glm::mat4(1.f);
			return  parMat * transformMatrix;
		}

		/**
	     * Get Normal Matrix
		 */
		//This only works if there is no non-uniform scale
		glm::mat3 getNormalMatrix() {
			if (transformMatrixDirty) {
				getTransformMatrix();
			}

			return (glm::mat3)(transformMatrix);
		}

};

#endif
