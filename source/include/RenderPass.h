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
    Framebuffer * fbo;
};

#endif