#include "RenderPass.h"
#include "Skybox.h"

SkyboxPass::SkyboxPass(Skybox* skybox) : skybox(skybox) {
}

void SkyboxPass::render() { if (skybox) skybox->draw(); }