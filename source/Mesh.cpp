#include "Mesh.h"
#include "GameObject.h"
#include "Shader.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>		// Post processing flags

#include "Animation.h"

#define INT_SIZE 4
#define FLOAT_SIZE 4

#define POSITION_COUNT 3
#define NORMAL_COUNT 3
#define TANGENT_COUNT 3
#define TEX_COORD_COUNT 2
#define BONE_ID_COUNT 4
#define BONE_WEIGHT_COUNT 4

#define VERTEX_ATTRIB_LOCATION 0
#define NORMAL_ATTRIB_LOCATION 1
#define TEX_COORD_0_ATTRIB_LOCATION 2
#define TANGENT_ATTRIB_LOCATION 3
#define BITANGENT_ATTRIB_LOCATION 4
#define BONE_WEIGHT_ATTRIB_LOCATION 5
#define BONE_ID_ATTRIB_LOCATION 6

std::unordered_map<std::string, MeshData> Mesh::meshMap;
std::unordered_map<std::string, BoneData>  Mesh::boneIdMap;


float tmpTime = 0;

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

	if (Renderer::currentShader == Renderer::getShader(FORWARD_PBR_SHADER_ANIM) && animationRoot) {
		BoneData & meshBoneData = boneIdMap.at(name);

		for (AnimNodeData node : animationRoot->data) {
			auto bone = meshBoneData.boneMap.find(node.name);
			if (bone == meshBoneData.boneMap.end()) continue;
			int id = bone->second;


			glm::mat4 transformMatrix = node.object->getTransformMatrix() * meshBoneData.boneBindArray[id];


			(*Renderer::currentShader)[std::string("bone_Matrix[") + std::to_string(id) + "]"] = transformMatrix;
		}
	}

	glDrawElements(GL_TRIANGLES, currentEntry.indexSize, GL_UNSIGNED_INT, 0);
}


bool boneWeightSort(std::pair<int, float> bone1, std::pair<int, float> bone2) {
	return bone1.second > bone2.second;
}

void loadMesh(std::string name, const aiMesh* mesh) {

	std::vector<float> megaArray;
	std::vector<int> idArray;
	std::vector<int> indexArray;

	bool enabledTexCoord[8];
	for (int t = 0; t < 8; ++t) {
		enabledTexCoord[t] = mesh->HasTextureCoords(t);
	}

	bool hasTangents = mesh->HasTangentsAndBitangents();

	std::pair<glm::ivec4, glm::vec4> *boneResults;

	if (mesh->HasBones()) {


		Mesh::boneIdMap[name] = BoneData();
		Mesh::boneIdMap[name].boneBindArray = std::vector<glm::mat4>(mesh->mNumBones);


		boneResults = new std::pair<glm::ivec4, glm::vec4>[mesh->mNumVertices];
		std::vector<std::pair<int, float>> *boneData = new std::vector<std::pair<int, float>>[mesh->mNumVertices];

		for (int b = 0; b < mesh->mNumBones; ++b) {
			Mesh::boneIdMap[name].boneMap[mesh->mBones[b]->mName.C_Str()] = b;
			for (int matIndex = 0; matIndex < 16; ++matIndex) {
				//Assimp matrices are row major, glm & opengl are column major, so we need to convert here
				Mesh::boneIdMap[name].boneBindArray[b][matIndex%4][matIndex/4] = mesh->mBones[b]->mOffsetMatrix[matIndex/4][matIndex%4];
			}
			
			for (int w = 0; w < mesh->mBones[b]->mNumWeights; ++w) {
				boneData[mesh->mBones[b]->mWeights[w].mVertexId].push_back(std::make_pair(b, mesh->mBones[b]->mWeights[w].mWeight));
			}
		}

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			std::sort(boneData[i].begin(), boneData[i].end(), boneWeightSort);
			glm::ivec4 bones(0,0,0,0);
			glm::vec4 weights(0,0,0,0);
			for (int d = 0; d < 4 && d < boneData[i].size(); ++d) {
				bones[d] = boneData[i][d].first;
				weights[d] = boneData[i][d].second;
			}
			weights = weights / (weights[0] + weights[1] + weights[2] + weights[3]);
			boneResults[i] = std::make_pair(bones, weights);
		}

		delete[] boneData;
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
		if (hasTangents) {
			for (int p = 0; p < TANGENT_COUNT; ++p) {
				megaArray.push_back(mesh->mTangents[i][p]);
			}
			for (int p = 0; p < TANGENT_COUNT; ++p) {
				megaArray.push_back(mesh->mBitangents[i][p]);
			}
		}
		if (mesh->HasBones()) {
			for (int p = 0; p < BONE_WEIGHT_COUNT; ++p) {
				megaArray.push_back(boneResults[i].second[p]);
			}
			for (int p = 0; p < BONE_ID_COUNT; ++p) {
				idArray.push_back(boneResults[i].first[p]);
			}
		}
	}

	if (mesh->HasBones()) delete[] boneResults;


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
	if (hasTangents) {
		glEnableVertexAttribArray(TANGENT_ATTRIB_LOCATION);
		glEnableVertexAttribArray(BITANGENT_ATTRIB_LOCATION);
	}
	if (mesh->HasBones()) {
		glEnableVertexAttribArray(BONE_ID_ATTRIB_LOCATION);
		glEnableVertexAttribArray(BONE_WEIGHT_ATTRIB_LOCATION);
	}

	GLuint meshBuffer[3];
	glGenBuffers(3, meshBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, megaArray.size() * sizeof(float), &(megaArray[0]), GL_STATIC_DRAW);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArray.size() * sizeof(int), &(indexArray[0]), GL_STATIC_DRAW);


	int stride = FLOAT_SIZE * (POSITION_COUNT + NORMAL_COUNT);
	if (hasTangents) stride += FLOAT_SIZE * (2 * TANGENT_COUNT);
	if (enabledTexCoord[0]) stride += FLOAT_SIZE * TEX_COORD_COUNT;
	if (mesh->HasBones()) stride += FLOAT_SIZE * BONE_WEIGHT_COUNT;

	GLuint currentOffset = 0;

	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (GLvoid*)currentOffset); currentOffset += (FLOAT_SIZE * 3);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (GLvoid*)currentOffset); currentOffset += (FLOAT_SIZE * 3);
	if (enabledTexCoord[0])	{
		glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, (GLvoid*)currentOffset); currentOffset += (FLOAT_SIZE * 2);
	}
	if (hasTangents) {
		glVertexAttribPointer(3, 3, GL_FLOAT, false, stride, (GLvoid*)currentOffset); currentOffset += (FLOAT_SIZE * 3);
		glVertexAttribPointer(4, 3, GL_FLOAT, false, stride, (GLvoid*)currentOffset); currentOffset += (FLOAT_SIZE * 3);
	}
	if (mesh->HasBones()) {
		glVertexAttribPointer(5, 4, GL_FLOAT, false, stride, (GLvoid*)currentOffset); currentOffset += (FLOAT_SIZE * 4);

		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer[2]);
		glBufferData(GL_ARRAY_BUFFER, idArray.size() * sizeof(int), &(idArray[0]), GL_STATIC_DRAW);
		glVertexAttribIPointer(6, 4, GL_INT, INT_SIZE * 4, (GLvoid*)0);
	}

	MeshData meshData;
	meshData.vaoHandle = vao;
	meshData.indexSize = static_cast<GLsizei>(indexArray.size());

	Mesh::meshMap[name] = meshData;
}

int counter = 0;

GameObject* parseNode(const aiScene* scene, aiNode* currentNode, std::string filename, std::unordered_map<std::string, Transform*>& loadingAcceleration) {
	GameObject* nodeObject = new GameObject();
	
	//add mesh to this object
	aiVector3D pos;
	aiVector3D scale;
	aiQuaternion rotate;

	currentNode->mTransformation.Decompose(scale, rotate, pos);
	
	nodeObject->transform.scale(scale.x);
	nodeObject->transform.translate(pos.x, pos.y, pos.z);
	nodeObject->transform.rotate(glm::quat(rotate.w, rotate.x, rotate.y, rotate.z));

	if (currentNode->mNumMeshes > 0) {
		std::string name = currentNode->mName.C_Str();
		if (name == "defaultobject") name = filename + std::to_string(counter);

		if (Mesh::meshMap.find(name) == Mesh::meshMap.end()) {
			int meshIndex = *currentNode->mMeshes;
			loadMesh(name, scene->mMeshes[meshIndex]);
		}
		
		nodeObject->addComponent(new Mesh(name));
	}

	loadingAcceleration[currentNode->mName.C_Str()] = &nodeObject->transform;

	//load child objects
	for (unsigned int c = 0; c < currentNode->mNumChildren; ++c) {
		nodeObject->addChild(*parseNode(scene, currentNode->mChildren[c], filename, loadingAcceleration));
	}
	return nodeObject;
}

void linkRoot(Animation* anim, Transform* currentTransform) {
	if (!currentTransform) return;

	Mesh* currentMesh;
	if ((currentMesh = currentTransform->gameObject->getComponent<Mesh>()) != nullptr) currentMesh->animationRoot = anim;
	for (Transform* child : currentTransform->children) {
		linkRoot(anim, child);
	}
}

GameObject* loadScene(const std::string& filename) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate | aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality |
        aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData |
        aiProcess_GenUVCoords | aiProcess_TransformUVCoords |
        aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace | aiProcess_CalcTangentSpace);

	if (!scene) {
        LOG(importer.GetErrorString());
        throw;
	}


	std::unordered_map<std::string, Transform*> loadingAcceleration;

	GameObject* retScene = parseNode(scene, scene->mRootNode, filename, loadingAcceleration);

	if (scene->HasAnimations()) {
		retScene->addComponent(new Animation(scene, loadingAcceleration));
		linkRoot(retScene->getComponent<Animation>(), &retScene->transform);
	}


    return retScene;
}
