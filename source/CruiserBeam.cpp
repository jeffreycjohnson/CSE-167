#include "CruiserBeam.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Renderer.h"

void CruiserBeam::loadCruiserBeam(GameObject* parent)
{
	Material* beamMaterial = new Material(Renderer::getShader(FORWARD_EMISSIVE));
	(*beamMaterial).transparent = true;
    (*beamMaterial)["colorTex"] = new Texture(glm::vec4(1.0, 0.25, 0.25, 0.5));
	parent->getComponent<Mesh>()->setMaterial(beamMaterial);
	CruiserBeam* addComponent = new CruiserBeam(parent);
	parent->addComponent<CruiserBeam>(addComponent);
}

CruiserBeam::CruiserBeam(GameObject* laserObj): laser(laserObj)
{
	firing = false;
	laser->visible = false;

	coolTime = (rand() / (float) RAND_MAX) * -2 * coolDuration;
}


CruiserBeam::~CruiserBeam()
{
}

void CruiserBeam::update(float dt) {
	if (firing) {
		fireTime += dt;
		if (fireTime > laserDuration) {
			fireTime = 0;
			firing = false;
			laser->visible = false;
		}
	} else {
		coolTime += dt;
		if (coolTime > coolDuration) {
			coolTime = 0;
			firing = true;
			laser->visible = true;
		}

	}
}
