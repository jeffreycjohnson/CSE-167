#pragma once
#include "ForwardDecs.h"
#include "Mesh.h"
class Skybox
{
public:
	static void draw();
	static GLuint loadCubemap(glm::mat4(&irradianceMatrix)[3], std::string imageFiles[6]);
private:
	struct ImageData {
		float* imageArray[6];
		int width[6];
		int height[6];
		int channels[6];
	};

	static bool loaded;
	static MeshData meshData;
	static void load();
	static GLuint loadGLCube(ImageData& data);
	static void loadIrradiance(glm::mat4(&irradianceMatrix)[3], ImageData& data);

	static glm::vec3 sampleTexture(ImageData& environment, glm::vec3 sampleDirection);
	static glm::vec3 SpecularEnvMap(glm::vec3 normal, float a, ImageData& environment);
};

