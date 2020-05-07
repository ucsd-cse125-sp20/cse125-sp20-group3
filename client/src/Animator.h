#pragma once

#include <string>

#include "../The-Forge/Middleware_3/Animation/AnimatedObject.h"
#include "../The-Forge/Middleware_3/Animation/SkeletonBatcher.h"
#include "../The-Forge/Middleware_3/Animation/Animation.h"
#include "../The-Forge/Middleware_3/Animation/Clip.h"
#include "../The-Forge/Middleware_3/Animation/ClipController.h"
#include "../The-Forge/Middleware_3/Animation/Rig.h"

#include "Transform.h"
#include "OzzGeode.h"

class Animator : public Transform {
public:
	// AnimatedObjects
	AnimatedObject gStickFigureAnimObject;

	// Animations
	Animation gAnimation;

	// ClipControllers
	ClipController gClipController;

	// Clips
	std::map<std::string, Clip*>* clips;

	// Rigs
	Rig gStickFigureRig;

	std::string directory;

	float time;
	bool updated = false;

	mat4** inverseBindPoses;
	uint32_t** jointRemaps;

	OzzObject::UniformDataBones boneData;

	Animator(OzzGeode* animatedGeode);
	Animator(OzzGeode* animatedGeode, mat4 transformation);

	~Animator();

	void SetClip(std::string clipName);

	void update(float deltaTime) override;
	
	void updateBoneBuffer(BufferUpdateDesc& desc, OzzObject::UniformDataBones* boneData);
};