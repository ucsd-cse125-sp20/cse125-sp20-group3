#pragma once

#include <string>

#include "../The-Forge/Common_3/Renderer/IRenderer.h"
#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"

#include "Object.h"

#define MAX_PARTICLES 1000
#define MAX_PARTICLE_SYSTEMS 100

class ParticleSystem : public Object {
public:
	struct ParticleData {
		float3 position;
		float2 scale;
		float4 color;
	};

	struct ParticleAuxData {
		float3 velocity;
		float life;
	};

	typedef void (*ParticleInitializer)(ParticleData* pd, ParticleAuxData* pad);
	typedef void (*ParticleUpdater)(ParticleData* pd, ParticleAuxData* pad, float deltaTime);

	struct ParticleSystemParams {
		std::string spriteFile;
		ParticleInitializer initializer = [](ParticleData* pd, ParticleAuxData* pad) {};
		ParticleUpdater updater = [](ParticleData* pd, ParticleAuxData* pad, float deltaTime) {};
		int numParticles = 100;
		float life = 1.0f;
	};

	Renderer* pRenderer;
	DescriptorSet* pDescriptorSetTexture = NULL;
	Buffer* pSpriteIndexBuffer = NULL;

	Texture* pSpriteTexture = NULL;
	ParticleData particleData[MAX_PARTICLES];
	ParticleAuxData particleAuxData[MAX_PARTICLES];
	ParticleSystemParams params;

	ParticleSystem(Renderer* renderer, ParticleSystemParams params);
	~ParticleSystem();

	void createSpriteResources(RootSignature* pRootSignature);

	void reset(float shift, float scale);
	void setInitilizer(ParticleInitializer initializer);
	void setLife(float life);

	void fillParticleData(ParticleData* buf);

	void update(float deltaTime) override;
	void draw(Cmd* cmd);
};