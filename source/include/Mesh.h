#ifndef INCLUDE_MESH_H
#define INCLUDE_MESH_H

#include "ForwardDecs.h"
#include "Component.h"
#include <unordered_map>

// returns the root game object of the file, with all children correctly added
GameObject* loadScene(const std::string& filename);

struct MeshData {
	GLuint vaoHandle;
    GLsizei indexSize;
	//BoundingBox boundingBox;
};

struct BoneData {
	std::vector<glm::mat4> boneBindArray;
	std::unordered_map<std::string, int> boneMap;
};

class Mesh : public Component
{
	public:
		static std::unordered_map<std::string, MeshData> meshMap;
		static std::unordered_map<std::string, BoneData> boneIdMap;

        std::string name;
		Animation* animationRoot;
    
        Mesh(std::string);
		~Mesh();

		void draw();
};

#endif