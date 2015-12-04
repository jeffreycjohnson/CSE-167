#include "include\ParticleTrail.h"
#include "GameObject.h"
#include "Transform.h"
#include "Renderer.h"

#define FLOAT_SIZE 4
#define VERTEX_ATTRIB_LOCATION 0
#define NORMAL_ATTRIB_LOCATION 1
#define DIR_ATTRIB_LOCATION 2

void ParticleTrail::uploadData(std::vector<float> &megaArray) {
	if (megaArray.size() > 0) {
		glBindVertexArray(vaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer);
		glBufferData(GL_ARRAY_BUFFER, megaArray.size() * sizeof(float), &(megaArray[0]), GL_STREAM_DRAW);
		int stride = FLOAT_SIZE * (3 + 3 + 1);
		int currentOffset = 0;
		glVertexAttribPointer(VERTEX_ATTRIB_LOCATION, 3, GL_FLOAT, false, stride, (GLvoid*)0); currentOffset += (FLOAT_SIZE * 3);
		glVertexAttribPointer(NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, false, stride, (GLvoid*)currentOffset); currentOffset += (FLOAT_SIZE * 3);
		glVertexAttribPointer(DIR_ATTRIB_LOCATION, 1, GL_FLOAT, false, stride, (GLvoid*)currentOffset); currentOffset += (FLOAT_SIZE * 1);
	}
}
ParticleTrail::ParticleTrail()
{
	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &meshBuffer);
	glBindVertexArray(vaoHandle);
	glEnableVertexAttribArray(VERTEX_ATTRIB_LOCATION);
	glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
	glEnableVertexAttribArray(DIR_ATTRIB_LOCATION);

}


ParticleTrail::~ParticleTrail()
{
}

void ParticleTrail::update(float dt) {
	waitTime += dt;
	if (waitTime > 1) {
		addPoint(gameObject->transform.getWorldPosition());
		uploadData(pointList);
	}
}

void ParticleTrail::draw()
{

	if (Renderer::gpuData.vaoHandle != vaoHandle) {
		glBindVertexArray(vaoHandle);
		Renderer::gpuData.vaoHandle = vaoHandle;
	}

	
	if (material) material->bind();

	//TODO move
	Renderer::setModelMatrix(glm::mat4(1));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, pointList.size()/3);
}

void ParticleTrail::addPoint(glm::vec3 point)
{
	int strideElements = (3 + 3 + 1);
	glm::vec3 normal (1,0,0);


	if (pointList.size() > 2*strideElements) {
		normal = point - glm::vec3(pointList[pointList.size() - 2 * strideElements], 
			                                      pointList[pointList.size() - 2 * strideElements + 1],
			                                      pointList[pointList.size() - 2 * strideElements + 2]);
		if (glm::length(normal) < 0.001) {
			normal = glm::vec3(pointList[pointList.size() - 2 * strideElements +3],
				pointList[pointList.size() - 2 * strideElements + 4],
				pointList[pointList.size() - 2 * strideElements + 5]);
		} else {
			normal = glm::normalize(normal);
		}
		if (isnan(normal.z)) throw;
	}
	for (int t = 0; t < 3; ++t) {
		pointList.push_back(point[t]);
	}
	for (int t = 0; t < 3; ++t) {
		pointList.push_back(normal[t]);
	}
	pointList.push_back(-1);

	for (int t = 0; t < 3; ++t) {
		pointList.push_back(point[t]);
	}
	for (int t = 0; t < 3; ++t) {
		pointList.push_back(normal[t]);
	}
	pointList.push_back(1);
}
