#include "Skybox.h"
#include "Renderer.h"

#define FLOAT_SIZE 4
#define POSITION_COUNT 3

#define VERTEX_ATTRIB_LOCATION 0

#define VERTEX_COUNT 12
float vertices[VERTEX_COUNT] = { -1, -1, 0,
1, -1, 0,
1,  1, 0,
-1,  1, 0 };

#define INDEX_COUNT 6
GLuint indices[INDEX_COUNT] = { 0, 1, 2, 0, 2, 3 };

bool Skybox::loaded = false;
MeshData Skybox::meshData;


void Skybox::load() {
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(VERTEX_ATTRIB_LOCATION);

	GLuint meshBuffer[2];
	glGenBuffers(2, meshBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * sizeof(float), vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_COUNT * sizeof(GLuint), indices, GL_STATIC_DRAW);


	int stride = FLOAT_SIZE * (POSITION_COUNT);
	glVertexAttribPointer(VERTEX_ATTRIB_LOCATION, 3, GL_FLOAT, false, stride, (GLvoid*)0);

	meshData.vaoHandle = vao;
	meshData.indexSize = static_cast<GLsizei>(INDEX_COUNT);
}

void Skybox::draw() {
	if (!loaded) {
		load();
	}

	if (Renderer::gpuData.vaoHandle != meshData.vaoHandle) {
		glBindVertexArray(meshData.vaoHandle);
		Renderer::gpuData.vaoHandle = meshData.vaoHandle;
	}

	glDrawElements(GL_TRIANGLES, meshData.indexSize, GL_UNSIGNED_INT, 0);
}
