#ifndef INCLUDE_MESH_H
#define INCLUDE_MESH_H

#include "ForwardDecs.h"
#include "Component.h"
#include <vector>
#include <unordered_map>

struct MeshData {
	int vaoHandle;
	int indexSize;
	//BoundingBox boundingBox;
};

class Mesh : public Component
{
	public:
		static std::unordered_map<std::string, MeshData> meshMap;

		std::string filename;

		Mesh(std::string);
		~Mesh();

		void loadObjFile(std::string);

		void draw(glm::mat4);
};

#endif