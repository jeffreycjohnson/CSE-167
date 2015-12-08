#include "Turret.h"
#include "Mesh.h"
#include "ObjectLoader.h"
#include "Renderer.h"
#include "Material.h"


Turret::Turret()
{
}


Turret::~Turret()
{
}

void Turret::update(float dt) {

}


Component* Turret::loadTurret(GameObject* parent)
{
	parent->removeComponent<Mesh>();
	GameObject* turretObj = loadScene("assets/turret.fbx");
	turretObj->transform.rotate(glm::angleAxis(atanf(1)*2.f, glm::vec3(1,0,0))); //convert back to z-up axis
	

	parent->addChild(*turretObj);
	Turret* addComponent = new Turret();
	return addComponent;
}
