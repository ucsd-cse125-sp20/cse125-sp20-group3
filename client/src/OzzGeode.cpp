#include "OzzGeode.h"

#include "Application.h"

OzzGeode::OzzGeode(Renderer* renderer, std::string directory)
{
	this->obj = conf_new(OzzObject, renderer, directory);
	this->radius = 5; // Arbitrary number

	waitForAllResourceLoads();
}

OzzGeode::~OzzGeode()
{
	unload();
	conf_delete(obj);
	obj = NULL;
}

void OzzGeode::unload()
{
	((OzzObject*)obj)->removeResources();
}

void OzzGeode::updateBoneBuffer(BufferUpdateDesc& desc, OzzObject::UniformDataBones* boneData)
{
	if (!boneData) boneData = &((OzzObject*)obj)->gUniformDataBones; // TODO move this into a transform node with a rig
	for (int i = 0; i < instanceIDs.size(); i++) {
		memcpy((mat4*)desc.pMappedData + instanceIDs[i] * MAX_NUM_BONES, boneData, sizeof(mat4) * ((OzzObject*)obj)->gStickFigureRig.GetNumJoints());
		//printf("%d=============================================================bones", i);
		//for (int j = 0; j < (int)((OzzObject*)obj)->gStickFigureRig.GetNumJoints(); j++) print(((mat4*)desc.pMappedData + instanceIDs[i])[j]);
	}
}

void OzzGeode::draw(Cmd* cmd)
{
	countingInstances = false;
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

		OzzGeode::MeshPushConstants pushConstants = {};
		pushConstants.nodeIndex = 0;
		pushConstants.instanceIndex = instanceID;
		pushConstants.modelIndex = ((GLTFObject*)obj)->modelID;
		pushConstants.animatedInstanceIndex = 0;

		//printf("%d %d %d\n", pushConstants.nodeIndex, pushConstants.instanceIndex, pushConstants.modelIndex);
		
		cmdBindPushConstants(cmd, this->shader.rootSignature, "cbRootConstants", &pushConstants);
		cmdBindVertexBuffer(cmd, 1, &((OzzObject*)obj)->pGeom->pVertexBuffers[0], ((OzzObject*)obj)->pGeom->mVertexStrides, (uint64_t*)NULL);
		cmdBindIndexBuffer(cmd, ((OzzObject*)obj)->pGeom->pIndexBuffer, ((OzzObject*)obj)->pGeom->mIndexType, (uint64_t)NULL);
		if (useMaterials)
			cmdBindDescriptorSet(cmd, 0, ((OzzObject*)obj)->pMaterialSet);
		cmdDrawIndexed(cmd, ((OzzObject*)obj)->pGeom->mIndexCount, 0, 0);
	}
}
