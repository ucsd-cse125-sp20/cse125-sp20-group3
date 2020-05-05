#pragma once

#include <string>

#include "../The-Forge/Common_3/Renderer/IRenderer.h"

#include "GLTFGeode.h"
#include "OzzObject.h"

#define MAX_ANIMATED_INSTANCES 500

class OzzGeode : public GLTFGeode {
public:
	OzzGeode(Renderer* renderer, std::string directory);
	~OzzGeode();

	void unload();

	void updateBoneBuffer(BufferUpdateDesc& desc, OzzObject::UniformDataBones* boneData) override;

	void draw(Cmd* cmd) override;
};