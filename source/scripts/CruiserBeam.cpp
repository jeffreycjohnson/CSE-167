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

	boom = new Sound("capital", false, false, 2, false);
	laserObj->addComponent(boom);

	startScale = laserObj->transform.getScale();
}


CruiserBeam::~CruiserBeam()
{
}

void CruiserBeam::update(float dt) {
	if (firing) {
		fireTime += dt;

		float scale = (1.0f - fireTime / laserDuration) + ((float) rand() / RAND_MAX * shake);
		gameObject->transform.setScale({ startScale.x * scale / 2, startScale.y / 2, startScale.z * scale / 2 });

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
			boom->play();
		}

	}
}
