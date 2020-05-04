#pragma once

#include <string>

#include "../The-Forge/Common_3/Renderer/IRenderer.h"

#include "GLTFGeode.h"
#include "OzzObject.h"

class OzzGeode : public GLTFGeode {
public:
	OzzGeode(Renderer* renderer, std::string directory);
	~OzzGeode();

	void updateBoneBuffer(BufferUpdateDesc& desc, OzzObject::UniformDataBones* boneData);

	void draw(Cmd* cmd) override;
};