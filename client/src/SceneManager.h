#pragma once

#include "Transform.h"
#include "GLTFGeode.h"

class SceneManager
{
public:
	Transform* root;

	// This vector should eventually be split between tracked objects and client only objects
	std::vector<Transform*> transforms;

	std::vector<Geode*> geodes;

	SceneManager();
	~SceneManager();
};