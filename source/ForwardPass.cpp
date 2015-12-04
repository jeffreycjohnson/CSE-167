#include "RenderPass.h"
#include "GameObject.h"
#include "Light.h"
#include "Mesh.h"
#include "Renderer.h"
#include <gtc/matrix_transform.hpp>

void ForwardPass::setObjects(const std::list<Component*>& list) {
	objectList = list;
}

void ForwardPass::setLights(const std::list<Light*>& list) {
	lightList = list;
}

void ForwardPass::render() {
	unsigned int lightIndex = 0;
	for (Light* l : lightList) {
		l->forwardPass(lightIndex++);
	}
	for (Component* c : objectList) {
        auto mesh = dynamic_cast<Mesh*>(c);
        if (mesh && mesh->material && !mesh->material->transparent) continue;
		c->draw();
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
            if (!mesh || (mesh->material && mesh->material->transparent)) continue;
            Material *mat = mesh->material;
            Shader *s = nullptr;
            if (mat) s = mat->shader;
            if (s == Renderer::getShader(DEFERRED_PBR_SHADER_ANIM)) mat->shader = Renderer::getShader(SHADOW_SHADER_ANIM);
            else if(mat) mat->shader = Renderer::getShader(SHADOW_SHADER);
            else Renderer::getShader(SHADOW_SHADER)->use();
            mesh->draw();
            if (mat) mat->shader = s;
        }
    }
}