#include "Mesh.h"
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

#define VERTEX_ATTRIB_LOCATION 0
#define NORMAL_ATTRIB_LOCATION 1
#define TEX_COORD_0_ATTRIB_LOCATION 2

std::unordered_map<std::string, MeshData> Mesh::meshMap;

Mesh::Mesh(std::string name) : name(name) {
    if (Mesh::meshMap.find(name) == Mesh::meshMap.end()) throw;
}

Mesh::~Mesh() {

}


void Mesh::draw() {
	MeshData& currentEntry = meshMap.at(name);

	if (Renderer::gpuData.vaoHandle != currentEntry.vaoHandle) {
		glBindVertexArray(currentEntry.vaoHandle);
		Renderer::gpuData.vaoHandle = currentEntry.vaoHandle;
	}

	glDrawElements(GL_TRIANGLES, currentEntry.indexSize, GL_UNSIGNED_INT, 0);
}

GameObject loadScene(const std::string& filename) {
	std::vector<float> megaArray;
	std::vector<int> indexArray;
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate | aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality |
        aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData |
        aiProcess_GenUVCoords | aiProcess_TransformUVCoords |
        aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace);

	if (!scene) {
        LOG(importer.GetErrorString());
        throw;
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

	glEnableVertexAttribArray(VERTEX_ATTRIB_LOCATION);
	glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
	if (enabledTexCoord[0]) {
		glEnableVertexAttribArray(TEX_COORD_0_ATTRIB_LOCATION);
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

	MeshData meshData;
	meshData.vaoHandle = vao;
	meshData.indexSize = static_cast<GLsizei>(indexArray.size());
    std::string name = filename;
    if (std::string("") != mesh->mName.C_Str())
        name = mesh->mName.C_Str();

    Mesh::meshMap[name] = meshData;

    GameObject ret;
    ret.addComponent(new Mesh(name));
    return ret;
}
