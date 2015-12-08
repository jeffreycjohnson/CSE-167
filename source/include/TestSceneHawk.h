#pragma once
#include "ForwardDecs.h"
#include "GameScene.h"
class TestSceneHawk : public Scene
{
public:
	TestSceneHawk();
	~TestSceneHawk();

	void loop() override;
	void debugDraw();
};

