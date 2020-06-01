#pragma once

#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"
#include "../The-Forge/Common_3/OS/Math/MathTypes.h"

#include "Object.h"
#include "OzzObject.h"

class Node : public Object
{
public:
	virtual void updateTransformBuffer(BufferUpdateDesc& desc, mat4 parentTransform, vec4 color) = 0;
	virtual void updateBoneBuffer(BufferUpdateDesc& desc, OzzObject::UniformDataBones* boneData) = 0;
	virtual void updateParticleBuffer(BufferUpdateDesc& desc) = 0;
	virtual void cull(const vec4 planes[6], bool doCull) = 0;
};