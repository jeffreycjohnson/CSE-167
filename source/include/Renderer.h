#ifndef INCLUDE_RENDERER_H
#define INCLUDE_RENDERER_H

#include "ForwardDecs.h"
#include "RenderPass.h"
#include "Shader.h"

#include <glfw3.h>

#define NEAR_DEPTH 0.2f
#define FAR_DEPTH 1000.f

#define SHADER_COUNT 14
#define FORWARD_PBR_SHADER_ANIM 0
#define FORWARD_PBR_SHADER 1
#define SKYBOX_SHADER 2
#define FBO_HDR 3
#define EMITTER_SHADER 4
#define EMITTER_BURST_SHADER 5
#define DEFERRED_PBR_SHADER_ANIM 6
#define DEFERRED_PBR_SHADER 7
#define DEFERRED_SHADER_LIGHTING 8
#define PARTICLE_TRAIL_SHADER 9
#define SHADOW_SHADER 10
#define SHADOW_SHADER_ANIM 11
#define BASIC_SHADER 12
#define FORWARD_UNLIT 13

#define FORWARD_SHADER_LIGHT_MAX 5

struct GPUData {
	int vaoHandle;
	int textureSlot[20];
};

class Renderer
{
	public:
		static Shader* currentShader;
		static Camera* camera;
		static GPUData gpuData;
        static std::list<RenderPass*> passes;

        static glm::mat4 perspective, view;

		static int shaderForwardLightList[2];

		static void init(int w, int h);
		static void loop();

		static void extractObjects();
		static void applyPerFrameData();
		static void updatePerspective(const glm::mat4& perspectiveMatrix);
		static void setIrradiance(glm::mat4(&irradianceMatrix)[3]);
		static void setEnvironment(int slot, float mipmapLevels);

		static void setCurrentShader(Shader*);
		static Shader& getCurrentShader();
		static Shader* getShader(int shaderId);
		static void switchShader(int shaderId);

		static void setModelMatrix(glm::mat4 transform);

		static int getWindowWidth() { return width; }
		static int getWindowHeight() { return height; }

		static void resize(int width, int height);

private:
    static int width, height;
	static float prevFOV;
    Renderer() = delete;
};

#endif