#ifndef INCLUDE_RENDERPASS_H
#define INCLUDE_RENDERPASS_H

#include "ForwardDecs.h"
#include "Shader.h"
#include "Skybox.h"

class RenderPass
{
public:
    virtual ~RenderPass() = default;
    virtual void render() = 0;
};

class ForwardPass : public RenderPass
{
public:
	void setObjects(const std::list<Component*>& list);
	void setLights(const std::list<Light*>& list);
    void render() override;
private:
	std::list<Component*> objectList;
	std::list<Light*> lightList;
};

class DeferredPass : public RenderPass
{
public:
    DeferredPass(int resx, int resy);
    ~DeferredPass() override;
    void render() override;
    Framebuffer * fbo;
};

class SkyboxPass : public RenderPass
{
public:
	SkyboxPass(Skybox* skybox);
	void render() override;
	Skybox* skybox;
};

#endif