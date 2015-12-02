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

    (*Renderer::currentShader)["uLightPosition"] = gameObject->transform.getWorldPosition();
    (*Renderer::currentShader)["uLightColor"] = color;
    (*Renderer::currentShader)["uLightSize"] = radius;
    (*Renderer::currentShader)["uLightDirection"] = gameObject->transform.getTransformMatrix() * glm::vec4(0, 0, -1, 0);
    glDrawElements(GL_TRIANGLES, currentEntry.indexSize, GL_UNSIGNED_INT, 0);
}

void PointLight::forwardPass()
{
}

void PointLight::deferredPass()
{
    (*Renderer::currentShader)["uLightType"] = 0;
    auto oldScale = gameObject->transform.scaleFactor;
    auto max = std::max(std::max(color.r, color.g), color.b);
    gameObject->transform.scaleFactor = glm::vec3((-linearFalloff + sqrtf(linearFalloff * linearFalloff - 4.0 * (constantFalloff - 256.0 * max) * exponentialFalloff))
        / (2.0 * exponentialFalloff));
    gameObject->transform.transformMatrixDirty = true;
    (*Renderer::currentShader)["uM_Matrix"] = gameObject->transform.getTransformMatrix();
    deferredHelper("Sphere");
    gameObject->transform.scaleFactor = oldScale;
    gameObject->transform.transformMatrixDirty = true;
}

void DirectionalLight::forwardPass()
{
}

void DirectionalLight::deferredPass()
{
    (*Renderer::currentShader)["uLightType"] = 1;
    (*Renderer::currentShader)["uM_Matrix"] = glm::mat4();
    deferredHelper("Plane");
}

void SpotLight::forwardPass()
{
}

void SpotLight::deferredPass()
{
}
