#include "GLTFGeode.h"

#include "Application.h"

bool GLTFGeode::useMaterials = true;

GLTFGeode::GLTFGeode(Renderer* renderer, std::string filename)
{
	this->obj = conf_new(GLTFObject);
	PathHandle modelFile = fsCopyPathInResourceDirectory(RD_MESHES, filename.c_str());
	GLTFObject::LoadModel((GLTFObject*)this->obj, renderer, modelFile);

	waitForAllResourceLoads();
}

GLTFGeode::~GLTFGeode()
{
	unload();
	conf_delete(obj);
}

void GLTFGeode::createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler)
{
	((GLTFObject*)obj)->createMaterialResources(pRootSignature, pBindlessTexturesSamplersSet, defaultSampler);
}

void GLTFGeode::unload()
{
	((GLTFObject*)obj)->removeResources();
}

void GLTFGeode::draw(Cmd* cmd)
{
	countingInstances = false;
	int instanceID = this->instanceIDs.front();
	instanceIDs.pop();

	cmdBindPipeline(cmd, shader.pipeline);
	for (int i = 0; i < DESCRIPTOR_UPDATE_FREQ_COUNT; i++) {
		if (shader.descriptorSets[i])
			cmdBindDescriptorSet(cmd, i == 0 ? 0 : Application::gFrameIndex, shader.descriptorSets[i]);
	}

	countingInstances = false;

	cmdBindVertexBuffer(cmd, 1, &((GLTFObject*)obj)->pGeom->pVertexBuffers[0], &((GLTFObject*)obj)->pGeom->mVertexStrides[0], NULL);
	cmdBindIndexBuffer(cmd, ((GLTFObject*)obj)->pGeom->pIndexBuffer, ((GLTFObject*)obj)->pGeom->mIndexType, 0);

	GLTFObject::MeshPushConstants pushConstants = {};
	pushConstants.nodeIndex = 0;
	pushConstants.instanceIndex = instanceID;
	pushConstants.modelIndex = ((GLTFObject*)obj)->modelID;

	for (uint32_t n = 0; n < ((GLTFObject*)obj)->pData->mNodeCount; ++n)
	{
		GLTFNode& node = ((GLTFObject*)obj)->pData->pNodes[n];
		if (node.mMeshIndex != UINT_MAX)
		{
			cmdBindPushConstants(cmd, this->shader.rootSignature, "cbRootConstants", &pushConstants);
			for (uint32_t i = 0; i < node.mMeshCount; ++i)
			{
				GLTFMesh& mesh = ((GLTFObject*)obj)->pData->pMeshes[node.mMeshIndex + i];

				if (useMaterials)
					cmdBindDescriptorSet(cmd, (uint32_t)((GLTFObject*)obj)->pData->pMaterialIndices[node.mMeshIndex + i], ((GLTFObject*)obj)->pMaterialSet);
				cmdDrawIndexed(cmd, mesh.mIndexCount, mesh.mStartIndex, 0);
			}
		}

		pushConstants.nodeIndex += 1;
	}
}
