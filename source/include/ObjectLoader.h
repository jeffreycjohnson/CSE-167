#pragma once
#include "ForwardDecs.h"

// returns the root game object of the file, with all children correctly added
GameObject* loadScene(const std::string& filename);

