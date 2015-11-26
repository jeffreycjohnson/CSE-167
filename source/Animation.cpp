#include "include\Animation.h"

#include "Transform.h"


glm::vec3 interpolateKeyframes(std::vector<std::pair<float, glm::vec3>> data, float time) {
	int positionIndex = 0;
	int numPositions = data.size();
	glm::vec3 currentPosition;

	for (int index = numPositions - 1; index >= 0; --index) {
		if (data[index].first < time) {
			positionIndex = index;
			break;
		}
	}
	if (positionIndex + 1 < numPositions) {
		float t = (time - data[positionIndex].first) / (data[positionIndex + 1].first - data[positionIndex].first);
		currentPosition = data[positionIndex].second * (1 - t) + (t)*data[positionIndex + 1].second;
	}
	else {
		currentPosition = data[positionIndex].second;
	}
	return currentPosition;
}

glm::quat interpolateQuaternions(std::vector<std::pair<float, glm::quat>> data, float time) {
	int rotationIndex = 0;
	int numRotations = data.size();
	glm::quat currentRotation;

	for (int index = numRotations - 1; index >= 0; --index) {
		if (data[index].first < time) {
			rotationIndex = index;
			break;
		}
	}
	if (rotationIndex + 1 < numRotations) {
		float t = (time - data[rotationIndex].first) / (data[rotationIndex + 1].first - data[rotationIndex].first);
		currentRotation = glm::slerp(data[rotationIndex].second, data[rotationIndex + 1].second, t);
	}
	else {
		currentRotation = data[rotationIndex].second;
	}
	return currentRotation;
}


void Animation::play(float dt) {
	currentTime += dt;
	for (AnimNodeData &node : data) {
		//TODO might want to place this elsewhere
		if (currentTime > node.animationTime) {
			currentTime -= node.animationTime;
		}

		node.object->position = interpolateKeyframes(node.keyframes.position, currentTime);
		node.object->rotation = interpolateQuaternions(node.keyframes.rotation, currentTime);
		//node.object->scaleFactor = node.keyframes.scale[scaleIndex].second;
		node.object->transformMatrixDirty = true;
	}
}

glm::quat convertQuat(aiQuaternion input) {
	return glm::quat(input.w, input.x, input.y, input.z);
}

glm::vec3 convertVec(aiVector3D input) {
	return glm::vec3(input.x, input.y, input.z);
}

Animation::Animation(const aiScene* scene, std::unordered_map<std::string, Transform*> loadingAcceleration)
{
	//TODO support multiple animaitons
	for (int a = 0; a < 1; ++a) { // scene->mNumAnimations; ++a) { //separate animations (e.g. run, jump)
		float longestTime = 0;
		for (int channel = 0; channel < scene->mAnimations[a]->mNumChannels; ++channel) {

			std::string name = scene->mAnimations[a]->mChannels[channel]->mNodeName.C_Str();

			AnimNodeData newData;
			newData.name = name;
			newData.object = loadingAcceleration[name];

			for (int keyframe = 0; keyframe < scene->mAnimations[a]->mChannels[channel]->mNumPositionKeys; ++keyframe) {
				newData.keyframes.position.push_back(std::make_pair(scene->mAnimations[a]->mChannels[channel]->mPositionKeys[keyframe].mTime,
					convertVec(scene->mAnimations[a]->mChannels[channel]->mPositionKeys[keyframe].mValue)));
			}

			for (int keyframe = 0; keyframe < scene->mAnimations[a]->mChannels[channel]->mNumRotationKeys; ++keyframe) {
				newData.keyframes.rotation.push_back(std::make_pair(scene->mAnimations[a]->mChannels[channel]->mRotationKeys[keyframe].mTime,
					convertQuat(scene->mAnimations[a]->mChannels[channel]->mRotationKeys[keyframe].mValue)));
			}

			for (int keyframe = 0; keyframe < scene->mAnimations[a]->mChannels[channel]->mNumScalingKeys; ++keyframe) {
				newData.keyframes.scale.push_back(std::make_pair(scene->mAnimations[a]->mChannels[channel]->mScalingKeys[keyframe].mTime,
					convertVec(scene->mAnimations[a]->mChannels[channel]->mScalingKeys[keyframe].mValue)));
			}

			if (newData.keyframes.position.back().first > longestTime) longestTime = newData.keyframes.position.back().first;
			if (newData.keyframes.rotation.back().first > longestTime) longestTime = newData.keyframes.rotation.back().first;
			if (newData.keyframes.scale.back().first > longestTime) longestTime = newData.keyframes.scale.back().first;

			data.push_back(newData);
			
		}
		for (AnimNodeData &nodeData : data) {
			nodeData.animationTime = longestTime;
		}
	}
}


Animation::~Animation()
{
}
