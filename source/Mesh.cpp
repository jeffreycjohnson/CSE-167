#include "Mesh.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "GameObject.h"
#include "Shader.h"

#include "Renderer.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>		// Post processing flags


#define FLOAT_SIZE 4
#define POSITION_COUNT 3
#define NORMAL_COUNT 3
#define TEX_COORD_COUNT 2


#define MV_MATRIX "uMV_Matrix"

std::unordered_map<std::string, MeshData> Mesh::meshMap;

Mesh::Mesh(std::string filename) : filename(filename) {
	loadObjFile(filename);
}

Mesh::~Mesh() {

}


void Mesh::draw(glm::mat4 cameraMat) {
	MeshData& currentEntry = meshMap.at(filename);
	Shader& currentShader = Renderer::getCurrentShader();

	if (Renderer::gpuData.vaoHandle != currentEntry.vaoHandle) {
		glBindVertexArray(currentEntry.vaoHandle);
		Renderer::gpuData.vaoHandle = currentEntry.vaoHandle;
	}

	currentShader[MV_MATRIX] = cameraMat * gameObject->transform.getTransformMatrix();

	glDrawElements(GL_TRIANGLES, currentEntry.indexSize, GL_UNSIGNED_INT, 0);
}





//Split functions from the interwebs
//http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

void Mesh::loadObjFile(std::string filename) {
	if (meshMap.find(filename) != meshMap.end()) {
		return;
	}

	std::vector<float> megaArray;
	std::vector<int> indexArray;


	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);


	if (!scene) {
		//TODO report error
		exit(0);
	}

	//TODO expand to multiple objects
	const aiMesh* mesh = scene->mMeshes[0];

	bool enabledTexCoord[8];
	for (int t = 0; t < 8; ++t) {
		enabledTexCoord[t] = mesh->HasTextureCoords(t);
	}

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		for (int p = 0; p < POSITION_COUNT; ++p) {
			megaArray.push_back(mesh->mVertices[i][p]);
		}
		for (int p = 0; p < NORMAL_COUNT; ++p) {
			megaArray.push_back(mesh->mNormals[i][p]);
		}
		if (enabledTexCoord[0]) {
			for (int p = 0; p < TEX_COORD_COUNT; ++p) {
				megaArray.push_back(mesh->mTextureCoords[0][i][p]);
			}
		}
	}
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		for (int p = 0; p < 3; ++p) {
			indexArray.push_back(mesh->mFaces[f].mIndices[p]);
		}
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(glGetAttribLocation(Renderer::getCurrentShader().id, "aPosition"));
	glEnableVertexAttribArray(glGetAttribLocation(Renderer::getCurrentShader().id, "aNormal"));
	if (enabledTexCoord[0]) {
		glEnableVertexAttribArray(glGetAttribLocation(Renderer::getCurrentShader().id, "aTexCoord"));
	}

	GLuint meshBuffer[2];
	glGenBuffers(2, meshBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, megaArray.size() * sizeof(float), &(megaArray[0]), GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArray.size() * sizeof(int), &(indexArray[0]), GL_STATIC_DRAW);


	int stride = FLOAT_SIZE * (POSITION_COUNT + NORMAL_COUNT + ((enabledTexCoord[0]) ? TEX_COORD_COUNT : 0));
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (GLvoid*)(FLOAT_SIZE * 3));
	if (enabledTexCoord[0])	glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, (GLvoid*)(FLOAT_SIZE * 6));

	
	//meshData.vertDataHandle = meshBuffer[0];
	//meshData.indexHandle = meshBuffer[1];

	MeshData meshData;
	meshData.vaoHandle = vao;
	meshData.indexSize = indexArray.size();
	meshMap[filename] = meshData;

}

