#ifndef INCLUDE_TRANSFORM_H
#define INCLUDE_TRANSFORM_H

#include "ForwardDecs.h"
#include <gtc/quaternion.hpp>
#include <vector>
#include "Component.h"

class Transform : public Component
{
	public:
		//parent Transform
		Transform* parent = nullptr;

		//child Transforms
		std::vector<Transform*> children;

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
        void translate(float x, float y, float z);
        void translate(glm::vec3& diff);
		void setPosition(float x, float y, float z);
		void setPosition(glm::vec3& pos);


		/**
		 * Rotate
		 * -Transform Dirty
		 * -Normals Dirty
		 */
		void setRotate(glm::quat& rotation);
        void rotate(glm::quat& diff);

		/**
		 * Scale
		 */
        void scale(float s);


		/**
		 * Get Transform Matrix
		 * -uses parent's matrix as well
		 */
        glm::mat4 getTransformMatrix();

		glm::vec3 getWorldPosition();
};

#endif
