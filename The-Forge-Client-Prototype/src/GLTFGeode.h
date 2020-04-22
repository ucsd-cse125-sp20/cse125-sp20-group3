#pragma once

#include "../The-Forge/Common_3/OS/Interfaces/IFileSystem.h"

#include "Geode.h"
#include "GLTFObject.h"

class GLTFGeode : public Geode
{
public:
	static bool useMaterials;

	GLTFGeode(Renderer* renderer, Sampler* defaultSampler, std::string filename);
	~GLTFGeode();

	void createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet);

	void unload() override;

	void draw(Cmd* cmd) override;
};