#pragma once
#include "ForwardDecs.h"
#include "Mesh.h"
class Skybox
{
public:
	static void draw();
	static GLuint loadCubemap(glm::mat4(&irradianceMatrix)[3], std::string imageFiles[6]);
private:
	static bool loaded;
	static MeshData meshData;
	static void load();
	static void loadIrradiance(glm::mat4(&irradianceMatrix)[3], std::string imageFiles[6]);
};

