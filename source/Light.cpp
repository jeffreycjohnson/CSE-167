#include "Light.h"
#include "Mesh.h"
#include "Renderer.h"
#include "GameObject.h"

void Light::deferredHelper(const std::string& meshName)
{
    // TODO : Bind falloff
    auto& currentEntry = Mesh::meshMap[meshName];

    if (Renderer::gpuData.vaoHandle != currentEntry.vaoHandle) {
        glBindVertexArray(currentEntry.vaoHandle);
        Renderer::gpuData.vaoHandle = currentEntry.vaoHandle;
    }

    Renderer::setModelMatrix(gameObject->transform.getTransformMatrix());
    (*Renderer::currentShader)["uLightPosition"] = gameObject->transform.getWorldPosition();
    (*Renderer::currentShader)["uLightColor"] = color;
    (*Renderer::currentShader)["uLightDirection"] = gameObject->transform.getTransformMatrix() * glm::vec4(0, 0, -1, 0);
    glDrawElements(GL_TRIANGLES, currentEntry.indexSize, GL_UNSIGNED_INT, 0);
}

void PointLight::forwardPass(int index)
{
	for (int shaderId : Renderer::shaderForwardLightList) {
		(*Renderer::getShader(shaderId))["uLightData[" + std::to_string(2*index) + "]"] = glm::vec4(gameObject->transform.position, 1.0);
		(*Renderer::getShader(shaderId))["uLightData[" + std::to_string(2*index+1) + "]"] = glm::vec4(color, 10);
	}
}

void PointLight::deferredPass()
{
    (*Renderer::currentShader)["uLightType"] = 0;
    auto oldScale = gameObject->transform.scaleFactor;
    // TODO : Make it use different falloffs
    gameObject->transform.scaleFactor = glm::vec3(16.0 * sqrtf(std::max(std::max(color.r, color.g), color.b)) * exponentialFalloff);
    gameObject->transform.transformMatrixDirty = true;
    (*Renderer::currentShader)["uM_Matrix"] = gameObject->transform.getTransformMatrix();
    gameObject->transform.scaleFactor = oldScale;
    gameObject->transform.transformMatrixDirty = true;
    deferredHelper("Sphere");
}

void DirectionalLight::forwardPass(int index)
{
}

void DirectionalLight::deferredPass()
{
    (*Renderer::currentShader)["uLightType"] = 1;
    (*Renderer::currentShader)["uM_Matrix"] = glm::mat4();
    deferredHelper("Plane");
}

void SpotLight::forwardPass(int index)
{
}

void SpotLight::deferredPass()
{
}
