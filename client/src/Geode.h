#pragma once

#include "../The-Forge/Common_3/Renderer/IRenderer.h"
#include "Node.h"
#include <queue>

#define MAX_GEOMETRY_INSTANCES 5000

class Geode : public Node 
{
public:
	struct GeodeShaderDesc {
		RootSignature* rootSignature;
		Pipeline* pipeline;
		DescriptorSet* descriptorSets[DESCRIPTOR_UPDATE_FREQ_COUNT];
	};

	Object* obj;
	GeodeShaderDesc shader = {};

	static bool countingInstances;
	static uint32_t instanceCount;
	int selfInstanceCount = 0;
	std::queue<int> instanceIDs;

	std::queue<vec3> positions;
	std::queue<bool> shouldCull;
	float radius = 0.0f;

	Geode();
	Geode(Object* obj);
	~Geode();

	void setProgram(GeodeShaderDesc desc);

	void update(float deltaTime) override;
	void updateTransformBuffer(BufferUpdateDesc& desc, mat4 parentTransform);
	void cull(const vec4 planes[6], bool doCull) override;
	void draw(Cmd* cmd) override;
};