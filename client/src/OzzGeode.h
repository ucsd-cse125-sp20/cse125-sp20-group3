#pragma once

#include <string>

#include "../The-Forge/Common_3/Renderer/IRenderer.h"

#include "GLTFGeode.h"
#include "OzzObject.h"

#define MAX_ANIMATED_INSTANCES 500

class OzzGeode : public GLTFGeode {
public:
	struct MeshPushConstants
	{
		uint32_t nodeIndex;
		uint32_t instanceIndex;
		uint32_t modelIndex;
		uint32_t animatedInstanceIndex;
	};

	int selfAnimatedInstanceCount = 0;

	OzzGeode(Renderer* renderer, std::string directory);
	~OzzGeode();

	void unload();

	void updateBoneBuffer(BufferUpdateDesc& desc, OzzObject::UniformDataBones* boneData) override;

	void draw(Cmd* cmd) override;
};