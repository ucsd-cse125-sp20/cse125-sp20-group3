#include "ParticleSystemGeode.h"
#include "Application.h"

bool ParticleSystemGeode::countingParticleInstances = false;
uint32_t ParticleSystemGeode::particleInstanceCount = 0;

ParticleSystemGeode::ParticleSystemGeode(Renderer* renderer, ParticleSystem::ParticleSystemParams params)
{
	this->obj = conf_new(ParticleSystem, renderer, params);
	this->radius = 5;

	waitForAllResourceLoads();
}

ParticleSystemGeode::~ParticleSystemGeode()
{
	conf_delete((ParticleSystem*)this->obj);
}

void ParticleSystemGeode::createSpriteResources(RootSignature* pRootSignature)
{
	((ParticleSystem*)this->obj)->createSpriteResources(pRootSignature);
}

void ParticleSystemGeode::updateParticleBuffer(BufferUpdateDesc& desc)
{
	if (!countingParticleInstances) {
		particleInstanceIDs = std::deque<int>();
		countingParticleInstances = true;
		particleInstanceCount = 0;
	}
	int particleID = particleInstanceCount++;
	particleInstanceIDs.push_back(particleID);
	((ParticleSystem*)this->obj)->fillParticleData(&((ParticleSystem::ParticleData*)desc.pMappedData)[particleID * (int)MAX_PARTICLES]);
}

void ParticleSystemGeode::update(float deltaTime)
{
	if (updated) return;
	updated = true;
	obj->update(deltaTime);
}

void ParticleSystemGeode::draw(Cmd* cmd)
{
	updated = false;
	countingInstances = false;
	countingParticleInstances = false;
	int particleID = particleInstanceIDs.front();
	particleInstanceIDs.pop_front();
	int instanceID = instanceIDs.front();
	instanceIDs.pop_front();
	bool culling = shouldCull.front();
	shouldCull.pop();

	if (!culling) {

		cmdBindPipeline(cmd, shader.pipeline);
		for (int i = 0; i < DESCRIPTOR_UPDATE_FREQ_COUNT; i++) {
			if (shader.descriptorSets[i])
				cmdBindDescriptorSet(cmd, i == 0 ? 0 : Application::gFrameIndex, shader.descriptorSets[i]);
		}

		MeshPushConstants pushConstants = {};
		pushConstants.instanceIndex = instanceID;
		pushConstants.particleInstanceIndex = particleID;

		cmdBindPushConstants(cmd, this->shader.rootSignature, "cbRootConstants", &pushConstants);
		this->obj->draw(cmd);

	}
}
