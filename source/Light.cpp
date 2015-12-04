#include "Light.h"
#include "Mesh.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Framebuffer.h"

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
    (*Renderer::currentShader)["uLightDirection"] = glm::vec3(gameObject->transform.getTransformMatrix() * glm::vec4(0, 0, -1, 0));
    glDrawElements(GL_TRIANGLES, currentEntry.indexSize, GL_UNSIGNED_INT, 0);
    CHECK_ERROR();
}

void PointLight::forwardPass(int index)
{
	for (int shaderId : Renderer::shaderForwardLightList) {
		(*Renderer::getShader(shaderId))["uLightData[" + std::to_string(2*index) + "]"] = glm::vec4(gameObject->transform.position, 1.0);
		(*Renderer::getShader(shaderId))["uLightData[" + std::to_string(2*index+1) + "]"] = glm::vec4(color, 1);
	}
}

void PointLight::deferredPass()
{
    (*Renderer::currentShader)["uLightType"] = 0;
    auto oldScale = gameObject->transform.scaleFactor;
    auto max = std::max(std::max(color.r, color.g), color.b);
    gameObject->transform.scaleFactor = glm::vec3((-linearFalloff + sqrtf(linearFalloff * linearFalloff - 4.0f * (constantFalloff - 256.0f * max) * exponentialFalloff))
        / (2.0f * exponentialFalloff));
    gameObject->transform.transformMatrixDirty = true;
    (*Renderer::currentShader)["uM_Matrix"] = gameObject->transform.getTransformMatrix();
    gameObject->transform.scaleFactor = oldScale;
    gameObject->transform.transformMatrixDirty = true;
    deferredHelper("Sphere");
}

DirectionalLight::DirectionalLight(bool shadow)
{
    if(shadow)
    {
        shadowCaster = shadow;
        fbo = new Framebuffer(2048, 2048, 0, true, false);
    }
}

DirectionalLight::~DirectionalLight()
{
    if(fbo) delete fbo;
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

void DirectionalLight::bindShadowMap()
{
    if(fbo && shadowCaster)
    {
        fbo->bind(0, nullptr);
    }
}

void SpotLight::forwardPass(int index)
{
}

void SpotLight::deferredPass()
{
}
