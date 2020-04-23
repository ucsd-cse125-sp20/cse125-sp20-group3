#pragma once

#include "../The-Forge/Common_3/Renderer/IRenderer.h"
#include "../The-Forge/Common_3/OS/Interfaces/IFileSystem.h"

#include "Geode.h"
#include "GLTFObject.h"

class GLTFGeode : public Geode
{
public:
	static bool useMaterials;

	GLTFGeode(Renderer* renderer, std::string filename);
	~GLTFGeode();

	void createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler);

	void unload();

	void draw(Cmd* cmd) override;
};