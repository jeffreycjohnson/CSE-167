#ifndef INCLUDE_RENDERPASS_H
#define INCLUDE_RENDERPASS_H

#include "ForwardDecs.h"
#include "Skybox.h"

class RenderPass
{
public:
    virtual ~RenderPass() = default;
    virtual void render(Camera*) = 0;
};

class ForwardPass : public RenderPass
{
public:
    void render(Camera* fbo) override;
};

class ParticlePass : public RenderPass
{
public:
    void render(Camera* fbo) override;
};

class DeferredPass : public RenderPass
{
public:
    DeferredPass();
    void render(Camera* fbo) override;
};

class SkyboxPass : public RenderPass
{
public:
	explicit SkyboxPass(Skybox* skybox);
    void render(Camera* fbo) override;
	Skybox* skybox;
};

class ShadowPass : public RenderPass
{
public:
    void render(Camera* fbo) override;
};

class BloomPass : public RenderPass
{
public:
    BloomPass();
    ~BloomPass() override;
    void render(Camera* fbo) override;

private:
    Framebuffer * brightPass;
    Framebuffer * blurBuffers[5];
};

#endif