#include "RenderPass.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Light.h"
#include "Framebuffer.h"
#include "Renderer.h"
#include <functional>

DeferredPass::DeferredPass(int resx, int resy)
{
    fbo = new Framebuffer(Renderer::getWindowWidth(), Renderer::getWindowHeight(), 4, true, true);

    (*Renderer::getShader(DEFERRED_SHADER_LIGHTING))["colorTex"] = 0;
    (*Renderer::getShader(DEFERRED_SHADER_LIGHTING))["normalTex"] = 1;
    (*Renderer::getShader(DEFERRED_SHADER_LIGHTING))["posTex"] = 2;
}

DeferredPass::~DeferredPass()
{
    if(fbo) delete fbo;
}

void DeferredPass::render()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_STENCIL_TEST);

    GLuint buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    fbo->bind(3, buffers);
    std::function<void(GameObject*)> gbufferPass;
    gbufferPass = [&](GameObject* obj)
    {
        auto mesh = obj->getComponent<Mesh>();
        if (mesh) {
            mesh->material->bind();
            mesh->draw();
        }

        for(auto child : obj->transform.children)
        {
            gbufferPass(child->gameObject);
        }
    };
    gbufferPass(&GameObject::SceneRoot);
    CHECK_ERROR();

    Renderer::getShader(DEFERRED_SHADER_LIGHTING)->use();
    (*Renderer::getShader(DEFERRED_SHADER_LIGHTING))["uScreenSize"] = glm::vec2(Renderer::getWindowWidth(), Renderer::getWindowHeight());
    glDepthMask(GL_FALSE);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
    glDrawBuffer(GL_COLOR_ATTACHMENT3);
    glClear(GL_COLOR_BUFFER_BIT);

    fbo->bindTexture(0, 0);
    fbo->bindTexture(1, 1);
    fbo->bindTexture(2, 2);
    (*Renderer::getShader(DEFERRED_SHADER_LIGHTING))["colorTex"] = 0;
    (*Renderer::getShader(DEFERRED_SHADER_LIGHTING))["normalTex"] = 1;
    (*Renderer::getShader(DEFERRED_SHADER_LIGHTING))["posTex"] = 2;

    std::function<void(GameObject*)> lightPass = [&](GameObject* obj)
    {
        auto l = obj->getComponent<Light>();
        if (l)
        {
            if (!dynamic_cast<DirectionalLight*>(l))
            {
                glDrawBuffer(GL_NONE);
                glDisable(GL_CULL_FACE);
                glEnable(GL_STENCIL_TEST);
                glEnable(GL_DEPTH_TEST);
                glClear(GL_STENCIL_BUFFER_BIT);
                glStencilFunc(GL_ALWAYS, 0, 0);

                l->deferredPass();


                glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
                glCullFace(GL_FRONT);
            }
            else
            {
                glCullFace(GL_BACK);
                glDisable(GL_STENCIL_TEST);
            }

            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glDrawBuffer(GL_COLOR_ATTACHMENT3);

            l->deferredPass();
        }

        for (auto child : obj->transform.children)
        {
            lightPass(child->gameObject);
        }
    };
    lightPass(&GameObject::SceneRoot);
    CHECK_ERROR();

    // TODO : Render Ambient and Environment
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
}
