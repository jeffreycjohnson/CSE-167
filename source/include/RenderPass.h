#ifndef INCLUDE_RENDERPASS_H
#define INCLUDE_RENDERPASS_H

#include "ForwardDecs.h"
#include "Shader.h"

class RenderPass
{
public:
    virtual ~RenderPass() = default;
    virtual void render() = 0;
};

class ForwardPass : public RenderPass
{
public:
    void render() override;
};

class DeferredPass : public RenderPass
{
public:
    DeferredPass(int resx, int resy);
    ~DeferredPass() override;
    void render() override;

private:
	//TODO uncomment me! // Shader shader = Shader("deferred_lighting.vert", "deferred_lighting.frag");
    Framebuffer * fbo;
};

#endif