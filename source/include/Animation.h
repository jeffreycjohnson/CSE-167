#pragma once
#include "ForwardDecs.h"
#include "Component.h"
#include <vector>
#include <unordered_map>

#include <gtc/quaternion.hpp>
#include <assimp/scene.h>           // Output data structure

struct KeyframeData {
	std::vector<std::pair<float, glm::vec3>> position;
	std::vector<std::pair<float, glm::quat>> rotation;
	std::vector<std::pair<float, glm::vec3>> scale;
};

struct AnimNodeData {
	std::string name;
	Transform* object;
	KeyframeData keyframes;
	float animationTime;
};

class Animation :
	public Component
{
public:
	std::vector<AnimNodeData> data;
	float currentTime=0;
	
	Animation(const aiScene*, std::unordered_map<std::string, Transform*>);
	~Animation();

	void play(float dt);
};

