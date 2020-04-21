#include "Geode.h"

#include "Application.h"

uint32_t Geode::instanceCount = 0;
bool Geode::countingInstances = false;

Geode::Geode(Object* obj)
{
	this->obj = obj;
}

Geode::~Geode()
{
}

void Geode::setRootSignature(GeodeShaderDesc desc)
{
	this->shader.rootSignature = desc.rootSignature;
	this->shader.pipeline = desc.pipeline;
	for (int i = 0; i < DESCRIPTOR_UPDATE_FREQ_COUNT; i++) {
		if (desc.descriptorSets[i])
			this->shader.descriptorSets[i] = desc.descriptorSets[i];
	}
}

void Geode::unload()
{
}

void Geode::update(float deltaTime)
{
	if (!countingInstances) {
		countingInstances = true;
		instanceCount = 0;
	}
	instanceID = instanceCount++;
	obj->update(deltaTime);
}

void Geode::updateTransformBuffer(BufferUpdateDesc& desc, mat4 parentTransform)
{
	countingInstances = false;
	mat4* instanceData = (mat4*)desc.pMappedData;
	instanceData[instanceID] = parentTransform;
}

void Geode::draw(Cmd* cmd)
{
	cmdBindPipeline(cmd, shader.pipeline);
	for (int i = 0; i < DESCRIPTOR_UPDATE_FREQ_COUNT; i++) {
		if (shader.descriptorSets[i])
			cmdBindDescriptorSet(cmd, i == 0 ? 0 : Application::gFrameIndex, shader.descriptorSets[i]);
	}
	cmdBindPushConstants(cmd, shader.rootSignature, "cbRootConstants", &instanceID);
	obj->draw(cmd);
}
