#pragma once

#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"
#include "../The-Forge/Common_3/OS/Math/MathTypes.h"

#include "Object.h"

class Node : public Object
{
public:
	virtual void unload() = 0;

	virtual void updateTransformBuffer(BufferUpdateDesc& desc, mat4 parentTransform) = 0;
};