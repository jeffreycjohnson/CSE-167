#include "RenderPass.h"
#include "GameObject.h"
#include "Light.h"

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
		c->draw();
	}
}