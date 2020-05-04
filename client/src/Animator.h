#pragma once

#include "../The-Forge/Middleware_3/Animation/AnimatedObject.h"
#include "../The-Forge/Middleware_3/Animation/SkeletonBatcher.h"
#include "../The-Forge/Middleware_3/Animation/Animation.h"
#include "../The-Forge/Middleware_3/Animation/Clip.h"
#include "../The-Forge/Middleware_3/Animation/ClipController.h"
#include "../The-Forge/Middleware_3/Animation/Rig.h"

#include "Transform.h"

class Animator : public Transform {
public:
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

	// Timer to get animationsystem update time
	HiresTimer gAnimationUpdateTimer;
};