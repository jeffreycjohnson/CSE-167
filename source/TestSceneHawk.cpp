#include "TestSceneHawk.h"
#include <SOIL.h>
#include "GameObject.h"
#include "Mesh.h"
#include "Camera.h"
#include "Skybox.h"
#include "Renderer.h"
#include "Texture.h"

#include "Animation.h"

Mesh* test2;
GameObject *hat, *bunny, *bagel, *barrel;
GameObject* turret;
GameObject *tankTop, *tankBot;
GameObject scene;
GameObject *bear;

Texture* hatTex, *turretTex, *bagelTex;
Texture* barrelTex, *barrelTex_Mat;
Texture* tankTexTop, *tankTexBot, *tankSpecTop, *tankSpecBot;
Texture* testNormal;
Texture* bearTex, *bearSpec;
Texture* blankNormal;

float tmp = 0;


TestSceneHawk::TestSceneHawk()
{


	hatTex =new Texture("assets/hat_tex.png");
	turretTex = new Texture("assets/turret_tex.png");
	bagelTex = new Texture("assets/bagel_everything_tex.png");


	barrelTex = new Texture("assets/WoodenBarrel/barrel_tex.png");

	barrelTex_Mat = new Texture("assets/WoodenBarrel/barrel_spec.png");



	tankTexTop = new Texture("assets/hover_tank/hoverTank1_top_tex.png");
	tankTexBot = new Texture("assets/hover_tank/hoverTank1_bot_tex.png");
	tankSpecTop = new Texture("assets/hover_tank/hoverTank1_top_spec.png");
	tankSpecBot = new Texture("assets/hover_tank/hoverTank1_bot_spec.png");

	testNormal = new Texture("assets/test_sphere_normal.png");

	bearTex = new Texture("assets/bearTex2.jpg");

	bearSpec = new Texture("assets/bearTex2_spec.png");


	blankNormal = new Texture("assets/blank_normal.png");

	scene.addChild(*Renderer::camera);

/*	bunny = loadScene("assets/bunny.obj");
	bunny->transform.scale(2);
	bunny->transform.translate(-2, 0, -10);
	*/
	Renderer::camera->transform.translate(0, 0, 20);
	/*
	turret = loadScene("assets/turret.dae");
	turret->transform.translate(-1, -2, 5);

	hat = loadScene("assets/hat.obj");
	hat->transform.translate(0, 1, 0);
	hat->transform.scale(5);
	bunny->addChild(*hat);

	bagel = loadScene("assets/bagel.obj");
	bagel->transform.scale(20);
	*/
	bear = loadScene("assets/bear2.dae");
	bear->transform.rotate(glm::angleAxis(atanf(1)*1.f, glm::vec3(1, 0, 0)));
	bear->transform.translate(0, -1, 1);

	barrel = loadScene("assets/test_sphere.obj");
	/*
	tankBot = loadScene("assets/hover_tank/hoverTank1_bot.obj");
	tankTop = loadScene("assets/hover_tank/hoverTank1_top.obj");*/
}

void TestSceneHawk::loop() {

	(*Renderer::getShader(FORWARD_PBR_SHADER))["normalTex"] = 2;
	testNormal->bindTexture(2);


	scene.transform.rotate(glm::angleAxis(0.001f, glm::vec3(0, 1, 0)));
	
	Renderer::switchShader(FORWARD_PBR_SHADER);
	(*Renderer::getShader(FORWARD_PBR_SHADER))["cameraPos"] = Renderer::camera->transform.getWorldPosition();


	barrel->transform.translate(5 * sin(tmp), 5 * cos(tmp), 4);
	(*Renderer::getShader(FORWARD_PBR_SHADER))["testMetal"] = 0 / 7.0f;
	(*Renderer::getShader(FORWARD_PBR_SHADER))["testRough"] = 0 / 7.0f;
	barrel->transform.scaleFactor *= 1 + sin(tmp / 5.f); //glm::vec3(2*sin(tmp / 5.f), 2 * sin(tmp / 5.f), 2 * sin(tmp / 5.f));
	barrel->draw();
	barrel->transform.scaleFactor = glm::vec3(1,1,1);
	barrel->transform.translate(-5 * sin(tmp), -5 * cos(tmp), -4);

	(*Renderer::getShader(FORWARD_PBR_SHADER))["uLightData[0]"] = glm::vec4(5 * sin(tmp), 5 * cos(tmp), 4, 1+sin(tmp/5.f));
	(*Renderer::getShader(FORWARD_PBR_SHADER))["uLightData[1]"] = glm::vec4(1, 1, 1, 10);

	for (int x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			float xDist = 2 * (x - 3.5);
			float yDist = 2 * (y - 3.5);
			float zDist = -1 + 2 * sin(x+y+tmp);
			barrel->transform.translate(xDist, yDist, zDist);
			(*Renderer::getShader(FORWARD_PBR_SHADER))["testMetal"] = (x) / 7.f;
			(*Renderer::getShader(FORWARD_PBR_SHADER))["testRough"] = (y) / 7.f;
			barrel->draw();
			barrel->transform.translate(-xDist, -yDist, -zDist);
		}
	}

	Renderer::switchShader(FORWARD_PBR_SHADER_ANIM);
	glActiveTexture(GL_TEXTURE0 + 2);
	(*Renderer::getShader(FORWARD_PBR_SHADER_ANIM))["colorTex"] = 0;
	(*Renderer::getShader(FORWARD_PBR_SHADER_ANIM))["matTex"] = 1;
	(*Renderer::getShader(FORWARD_PBR_SHADER_ANIM))["normalTex"] = 2;
	bearTex->bindTexture(0);
	bearSpec->bindTexture(1);
	blankNormal->bindTexture(2);

	(*Renderer::getShader(FORWARD_PBR_SHADER_ANIM))["cameraPos"] = Renderer::camera->transform.getWorldPosition();

	(*Renderer::getShader(FORWARD_PBR_SHADER_ANIM))["uLightData[0]"] = glm::vec4(5 * sin(tmp), 5 * cos(tmp), 4, 1 + sin(tmp / 5.f));
	(*Renderer::getShader(FORWARD_PBR_SHADER_ANIM))["uLightData[1]"] = glm::vec4(1, 1, 1, 10);
	(*Renderer::getShader(FORWARD_PBR_SHADER_ANIM))["useTextures"] = true;

	bear->getComponent<Animation>()->play(0.032);
	
	bear->draw();
	Renderer::switchShader(FORWARD_PBR_SHADER);

	tmp += 0.02f;

/*	turret->transform.translate(0.2f*sin(tmp), 0, 0);

	////Turret targeting code
	glm::vec3 target = bagel->transform.getWorldPosition();


	glm::vec3 turretNormal = glm::normalize(glm::vec3(turret->transform.getTransformMatrix() * glm::vec4(0, 0, 1, 0)));
	glm::vec3 turretX = glm::normalize(glm::vec3(turret->transform.getTransformMatrix() * glm::vec4(1, 0, 0, 0)));
	glm::vec3 turretZ = glm::cross(turretX, turretNormal);

	glm::mat3 turretBasis(turretX, turretNormal, turretZ);


	glm::vec3 turretPos(turret->transform.getWorldPosition());
	glm::vec3 diff;
	diff.x = target.x - turretPos.x;
	diff.y = target.y - turretPos.y;
	diff.z = target.z - turretPos.z;

	diff = turretBasis * diff;
	float yaw = atan2f(diff.x, diff.z);
	float pitch = -atan2f(diff.y, sqrtf(diff.x*diff.x + diff.z*diff.z));

	turret->transform.children[0]->children[0]->setRotate(glm::angleAxis(yaw, glm::vec3(0, 0, 1)));
	turret->transform.children[0]->children[0]->children[0]->setRotate(glm::angleAxis(pitch, glm::vec3(1, 0, 0)));
	////-----------

	*/
}

TestSceneHawk::~TestSceneHawk()
{
}
