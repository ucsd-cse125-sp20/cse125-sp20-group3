#pragma once

#include "Geode.h"
#include "ParticleSystem.h"

class ParticleSystemGeode : public Geode {
public:

	struct MeshPushConstants
	{
		uint32_t instanceIndex;
		uint32_t particleInstanceIndex;
	};

	static bool countingParticleInstances;
	static uint32_t particleInstanceCount;
	int selfParticleInstanceCount = 0;
	std::deque<int> particleInstanceIDs;

	ParticleSystemGeode(Renderer* renderer, ParticleSystem::ParticleSystemParams params);
	~ParticleSystemGeode();

	void updateParticleBuffer(BufferUpdateDesc& desc) override;

	void createSpriteResources(RootSignature* pRootSignature);

	void draw(Cmd* cmd) override;
};