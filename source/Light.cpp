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
    (*Renderer::currentShader)["uLightDirection"] = gameObject->transform.getTransformMatrix() * glm::vec4(0,0,-1,0);
    glDrawElements(GL_TRIANGLES, currentEntry.indexSize, GL_UNSIGNED_INT, 0);
}

void PointLight::forwardPass()
{
}

void PointLight::deferredPass()
{
    deferredHelper("Sphere");
}

void DirectionalLight::forwardPass()
{
}

void DirectionalLight::deferredPass()
{
    deferredHelper("Plane");
}

void SpotLight::forwardPass()
{
}

void SpotLight::deferredPass()
{
}
