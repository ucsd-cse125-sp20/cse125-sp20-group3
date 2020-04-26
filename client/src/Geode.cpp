#include "Geode.h"

#include "Application.h"

uint32_t Geode::instanceCount = 0;
bool Geode::countingInstances = false;

Geode::Geode()
{
}

Geode::Geode(Object* obj)
{
	this->obj = obj;
}

Geode::~Geode()
{
}

void Geode::setProgram(GeodeShaderDesc desc)
{
	this->shader.rootSignature = desc.rootSignature;
	this->shader.pipeline = desc.pipeline;
	for (int i = 0; i < DESCRIPTOR_UPDATE_FREQ_COUNT; i++) {
		if (desc.descriptorSets[i])
			this->shader.descriptorSets[i] = desc.descriptorSets[i];
	}
}

void Geode::update(float deltaTime)
{
	obj->update(deltaTime);
}

void Geode::updateTransformBuffer(BufferUpdateDesc& desc, mat4 parentTransform)
{
	if (!countingInstances) {
		instanceIDs = std::queue<int>();
		positions = std::queue<vec3>();
		shouldCull = std::queue<bool>();
		countingInstances = true;
		instanceCount = 0;
	}
	int instanceID = instanceCount++;
	instanceIDs.push(instanceID);
	positions.push(parentTransform[3].getXYZ());
	mat4* instanceData = (mat4*)desc.pMappedData;
	instanceData[instanceID] = parentTransform;
}

void Geode::cull(const vec4 planes[6], bool doCull)
{
	countingInstances = false;
	if (!doCull) {
		shouldCull.push(false);
		return;
	}
	vec3 position = positions.front();
	positions.pop();

	for (int i = 0; i < 6; ++i)
	{
		float distance = dot(position, planes[i].getXYZ()) + planes[i].getW();

		if (distance < -radius) {
			shouldCull.push(true);
			return;
		}
	}
	shouldCull.push(false);
}

void Geode::draw(Cmd* cmd)
{
	int instanceID = instanceIDs.front();
	instanceIDs.pop();
	bool culling = shouldCull.front();
	shouldCull.pop();

	if (!culling) {
		cmdBindPipeline(cmd, shader.pipeline);
		for (int i = 0; i < DESCRIPTOR_UPDATE_FREQ_COUNT; i++) {
			if (shader.descriptorSets[i])
				cmdBindDescriptorSet(cmd, i == 0 ? 0 : Application::gFrameIndex, shader.descriptorSets[i]);
		}
		cmdBindPushConstants(cmd, shader.rootSignature, "cbRootConstants", &instanceID);
		obj->draw(cmd);
	}
}
