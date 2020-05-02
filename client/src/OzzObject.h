#pragma once

#include <string>

#include "Object.h"

#include "../The-Forge/Common_3/OS/Interfaces/IFileSystem.h"
#include "../The-Forge/Common_3/OS/Interfaces/ITime.h"
#include "../The-Forge/Common_3/Renderer/IRenderer.h"
#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"

#include "../The-Forge/Middleware_3/Animation/AnimatedObject.h"
#include "../The-Forge/Middleware_3/Animation/SkeletonBatcher.h"
#include "../The-Forge/Middleware_3/Animation/Animation.h"
#include "../The-Forge/Middleware_3/Animation/Clip.h"
#include "../The-Forge/Middleware_3/Animation/ClipController.h"
#include "../The-Forge/Middleware_3/Animation/Rig.h"

#define MAX_NUM_BONES 200

class OzzObject : public Object{//GLTFObject {
public:

	struct UniformDataBones
	{
		mat4 mBoneMatrix[MAX_NUM_BONES];
	};

	// AnimatedObjects
	AnimatedObject gStickFigureAnimObject;

	// Animations
	Animation gAnimation;

	// ClipControllers
	ClipController gClipController;

	// Clips
	Clip gClip;

	// Rigs
	Rig gStickFigureRig;

	// SkeletonBatcher
	SkeletonBatcher gSkeletonBatcher;

	// Timer to get animationsystem update time
	HiresTimer gAnimationUpdateTimer;

	static VertexLayout pVertexLayoutSkinned;

	std::string directory;
	float time;

	Geometry* pGeom = NULL;
	UniformDataBones gUniformDataBones;
	Texture* pTextureDiffuse = NULL;

	OzzObject(std::string directory);

	// TODO Make better
	void AddClip(std::string clipName);

	void removeResources();

	static VertexLayout getVertexLayout() { return pVertexLayoutSkinned; }

	void update(float deltaTime) override;

	void draw(Cmd* cmd) override;
};