#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(Renderer* renderer, ParticleSystemParams params) {
	this->params = params;
	this->pRenderer = renderer;

	// Index buffer
	uint16_t indices[] = {
		0, 2, 1, 1, 2, 3
	};
	BufferLoadDesc spriteIBDesc = {};
	spriteIBDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_INDEX_BUFFER;
	spriteIBDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
	spriteIBDesc.mDesc.mSize = sizeof(indices);
	spriteIBDesc.pData = indices;
	spriteIBDesc.ppBuffer = &pSpriteIndexBuffer;
	addResource(&spriteIBDesc, NULL, LOAD_PRIORITY_NORMAL);

	// Sprites texture
	PathHandle spritesPath = fsCopyPathInResourceDirectory(RD_TEXTURES, params.spriteFile.c_str());
	TextureLoadDesc textureDesc = {};
	textureDesc.ppTexture = &pSpriteTexture;
	textureDesc.pFilePath = spritesPath;
	addResource(&textureDesc, NULL, LOAD_PRIORITY_NORMAL);

	for (int i = 0; i < params.numParticles; i++) {
		particleAuxData[i].life = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / params.life);
		particleData[i].color = float4(0);
	}

	waitForAllResourceLoads();
}

ParticleSystem::~ParticleSystem()
{
	removeResource(pSpriteTexture);
	removeResource(pSpriteIndexBuffer);

	removeDescriptorSet(pRenderer, pDescriptorSetTexture);
}

void ParticleSystem::createSpriteResources(RootSignature* pRootSignature)
{
	// Use one set per material.
	DescriptorSetDesc desc = {};
	desc.mUpdateFrequency = DESCRIPTOR_UPDATE_FREQ_NONE;
	desc.pRootSignature = pRootSignature;
	desc.mMaxSets = 1;
	addDescriptorSet(pRenderer, &desc, &pDescriptorSetTexture);

	DescriptorData params[1] = {};
	params[0].pName = "uTexture0";
	params[0].ppTextures = &pSpriteTexture;
	updateDescriptorSet(pRenderer, 0, pDescriptorSetTexture, 1, params);
}

void ParticleSystem::updateInitilizer(ParticleInitializer initializer)
{
	params.initializer = initializer;
}

void ParticleSystem::fillParticleData(ParticleData* buf)
{
	memcpy(buf, particleData, params.numParticles * sizeof(ParticleData));
}

void ParticleSystem::update(float deltaTime)
{
	//printf("updating====================================================\n");
	for (int i = 0; i < params.numParticles; i++) {
		if (particleAuxData[i].life > params.life) {
			params.initializer(&particleData[i], &particleAuxData[i]);
			particleAuxData[i].life = 0.0f;
		}
		if (particleAuxData[i].life > 0) {
			params.updater(&particleData[i], &particleAuxData[i], deltaTime);
			particleData[i].position += particleAuxData[i].velocity * deltaTime;
		}
		particleAuxData[i].life += deltaTime;
		//printf("%f : ", particleAuxData[i].life);
		//printf("%f : ", particleData[i].scale);
		//print(particleData[i].position);
	}
}

void ParticleSystem::draw(Cmd* cmd)
{
	cmdBindDescriptorSet(cmd, 0, pDescriptorSetTexture);
	cmdBindIndexBuffer(cmd, pSpriteIndexBuffer, INDEX_TYPE_UINT16, 0);
	cmdDrawIndexedInstanced(cmd, 6, 0, params.numParticles, 0, 0);
}
