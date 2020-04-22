#ifndef _POINT_CLOUD_H_
#define _POINT_CLOUD_H_

#include <vector>
#include <string>
#include <iostream>

#include "../The-Forge/Common_3/Renderer/IRenderer.h"
#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"
#include "../The-Forge/Common_3/OS/Interfaces/IProfiler.h"

#include "Object.h"

class OBJObject : public Object
{
public:

	struct UniformBlock {
		mat4 mProjectView;
		mat4 mToWorld;
		vec4 mColor;
	};

	struct VertexData {
		vec4 position;
		vec4 normal;
		vec2 uv;
	};

private:
	vec3 color;

	std::vector<VertexData> points;
	int size;

	Shader* program;
	RootSignature* pRootSignature;
	DescriptorSet* pDescUniforms;
	DescriptorSet* pDescTexture;

	UniformBlock uniformData;
	Buffer* pUniformBuffer[3] = { NULL };

	Buffer* pVertexBuffer = NULL;
	Pipeline* pPipeline = NULL;

	Texture* pTexture = NULL;

	ProfileToken profileToken = -1;

public:

	OBJObject(std::string objFilename, RootSignature* rootSignature, DescriptorSet* descriptorSet);
	void prep(Renderer* renderer);
	~OBJObject();

	void parse(const char* filename);
	void loadTexure(std::string filename, DescriptorSet* texDescSet);
	void profile(ProfileToken profileToken);

	void load(Renderer* renderer, RenderTarget* target, RenderTarget* depthBuffer, Shader* program);
	void unload(Renderer* renderer);
	void draw(Cmd* commands) override;
	void update(float deltaTime) override;

	void setTranslate(vec3 position);
	void setScaleRot(vec3 scale, float deg, vec3 axis);

	void setPositionDirection(vec3 position, vec3 direction, vec3 up);
	void setPositionDirection(vec3 position, vec3 direction);

	UniformBlock getUniformData() { return uniformData; }
};

#endif

                                                                                                                                                                                                                                                                                                                                                                                      