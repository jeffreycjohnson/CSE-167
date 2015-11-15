#pragma once
#include "ForwardDecs.h"
#include "Mesh.h"
class Skybox
{
public:
	static void draw();
private:
	static bool loaded;
	static MeshData meshData;
	static void load();
};

