#ifndef INCLUDE_RENDERER_H
#define INCLUDE_RENDERER_H

#include "ForwardDecs.h"
#include "RenderPass.h"
#include "Shader.h"

#include <glfw3.h>

#define SHADER_COUNT 4
#define FORWARD_PBR_SHADER_ANIM 0
#define FORWARD_PBR_SHADER 1
#define SKYBOX_SHADER 2
#define FBO_HDR 3

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

		static void init(int w, int h);
		static void loop();

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

		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void resize(int width, int height);
		static void window_focus_callback(GLFWwindow* window, int focused);

		//glfwSetScrollCallback(window, scroll_callback);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
    static int width, height;
    Renderer() = delete;
};

#endif