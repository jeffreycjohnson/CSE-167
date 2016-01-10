#include "RenderPass.h"
#include "GameObject.h"
#include "Light.h"
#include "Mesh.h"
#include "Renderer.h"

void ForwardPass::setObjects(const std::list<Component*>& list) {
	objectList = list;
}

void ForwardPass::setLights(const std::list<Light*>& list) {
	lightList = list;
}

void ForwardPass::render() {
	unsigned int lightIndex = 0;
	//TODO sort lights by importance?
	for (Light* l : lightList) {
		if (lightIndex > FORWARD_SHADER_LIGHT_MAX) break;
		l->forwardPass(lightIndex++);
	}
	for (Component* c : objectList) {
        auto mesh = dynamic_cast<Mesh*>(c);
        if (mesh && mesh->material && !mesh->material->transparent) continue;
		if (mesh && mesh->gameObject && !mesh->gameObject->visible) continue;
        if(mesh && mesh->material && (mesh->material->shader == Renderer::getShader(FORWARD_UNLIT) || mesh->material->shader == Renderer::getShader(FORWARD_EMISSIVE))) glDepthMask(GL_FALSE);
        if(mesh && mesh->material) mesh->material->bind();
		c->draw();
        if (mesh && mesh->material && (mesh->material->shader == Renderer::getShader(FORWARD_UNLIT) || mesh->material->shader == Renderer::getShader(FORWARD_EMISSIVE))) glDepthMask(GL_TRUE);
	}
}

void ShadowPass::render()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_STENCIL_TEST);

    for (Light* l : lightList) {
        auto caster = dynamic_cast<DirectionalLight*>(l);
        if (!caster || !caster->shadowCaster) continue;
        caster->bindShadowMap();
        glDrawBuffer(GL_NONE);
        for (Component* c : objectList) {
            auto mesh = dynamic_cast<Mesh*>(c);
            if (!mesh || (mesh->material && mesh->material->transparent) || !mesh->visible) continue;
            Material* mat = mesh->material;
            Shader* s = nullptr;
            if (mat->shader == Renderer::getShader(DEFERRED_PBR_SHADER_ANIM)) s = Renderer::getShader(SHADOW_SHADER_ANIM);
            else s = Renderer::getShader(SHADOW_SHADER);
            if (s != Renderer::currentShader) s->use();
            mesh->draw();
        }
    }
}