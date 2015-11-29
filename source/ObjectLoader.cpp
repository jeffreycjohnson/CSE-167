#include "ObjectLoader.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Shader.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>		// Post processing flags

#include "Animation.h"


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
			Mesh::loadMesh(name, scene->mMeshes[meshIndex]);
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
