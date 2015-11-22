#ifndef INCLUDE_RENDERPASS_H
#define INCLUDE_RENDERPASS_H

#include "ForwardDecs.h"

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
    void render() override;
};

#endif