#ifndef INCLUDE_RENDERPASS_H
#define INCLUDE_RENDERPASS_H

#include "ForwardDecs.h"
#include "Shader.h"
#include "Skybox.h"
#include <list>

class RenderPass
{
public:
    virtual ~RenderPass() = default;
    virtual void render() = 0;
};

class ForwardPass : public RenderPass
{
public:
    virtual void render() override;
};

class ParticlePass : public RenderPass
{
public:
    virtual void render() override;
};

class DeferredPass : public RenderPass
{
public:
    DeferredPass();
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

class ShadowPass : public ForwardPass
{
public:
    void render() override;
};

class BloomPass : public RenderPass
{
public:
    BloomPass(DeferredPass * deferred);
    ~BloomPass() override;
    void render() override;

private:
    Framebuffer * brightPass;
    Framebuffer * blurBuffers[5];
    DeferredPass * deferredPass;
};

#endif