#include "OzzObject.h"
#include "Application.h"

VertexLayout OzzObject::pVertexLayoutSkinned = {
	5,
	{
		{SEMANTIC_POSITION, 0, NULL, TinyImageFormat_R32G32B32_SFLOAT, 0, 0, 0, 0},
		{SEMANTIC_NORMAL, 0, NULL, TinyImageFormat_R32G32B32_SFLOAT, 1, 0, 3 * sizeof(float), 0},
		{SEMANTIC_TEXCOORD0, 0, NULL, TinyImageFormat_R32G32_SFLOAT, 2, 0, 6 * sizeof(float), 0},
		{SEMANTIC_WEIGHTS, 0, NULL, TinyImageFormat_R32G32B32A32_SFLOAT, 3, 0, 8 * sizeof(float), 0},
		{SEMANTIC_JOINTS, 0, NULL, TinyImageFormat_R16G16B16A16_UINT, 4, 0, 12 * sizeof(float), 0}
	}
};

OzzObject::OzzObject(std::string directory)
{
	this->directory = directory;

	PathHandle skeletonPath = fsCopyPathInResourceDirectory(RD_ANIMATIONS, (directory + "/skeleton.ozz").c_str());
	
	// Initialize the rig with the path to its ozz file
	gStickFigureRig.Initialize(skeletonPath);

	// Add the rig to the list of skeletons to render
	gSkeletonBatcher.AddRig(&gStickFigureRig);



	PathHandle meshPath = fsCopyPathInResourceDirectory(RD_ANIMATIONS, (directory + "/riggedMesh.gltf").c_str());

	OzzObject::pVertexLayoutSkinned.mAttribCount = 5;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mSemantic = SEMANTIC_POSITION;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mLocation = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mOffset = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mSemantic = SEMANTIC_NORMAL;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mLocation = 1;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mOffset = 3 * sizeof(float);
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mSemantic = SEMANTIC_TEXCOORD0;
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mFormat = TinyImageFormat_R32G32_SFLOAT;
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mLocation = 2;
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mOffset = 6 * sizeof(float);
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mSemantic = SEMANTIC_WEIGHTS;
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mFormat = TinyImageFormat_R32G32B32A32_SFLOAT;
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mLocation = 3;
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mOffset = 8 * sizeof(float);
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mSemantic = SEMANTIC_JOINTS;
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mFormat = TinyImageFormat_R16G16B16A16_UINT;
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mLocation = 4;
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mOffset = 12 * sizeof(float);

	GeometryLoadDesc loadDesc = {};
	loadDesc.pFilePath = meshPath;
	loadDesc.pVertexLayout = &OzzObject::pVertexLayoutSkinned;
	loadDesc.ppGeometry = &pGeom;
	addResource(&loadDesc, NULL, LOAD_PRIORITY_NORMAL);

	PathHandle texturePath = fsCopyPathInResourceDirectory(RD_TEXTURES, (directory + "_tex.png").c_str());
	TextureLoadDesc diffuseTextureDesc = {};
	diffuseTextureDesc.pFilePath = texturePath;
	diffuseTextureDesc.ppTexture = &pTextureDiffuse;
	addResource(&diffuseTextureDesc, NULL, LOAD_PRIORITY_NORMAL);

	//SkeletonRenderDesc skeletonRenderDesc = {};
	//gSkeletonBatcher.Initialize(skeletonRenderDesc);
}

void OzzObject::AddClip(std::string clipName)
{
	PathHandle clipPath = fsCopyPathInResourceDirectory(RD_ANIMATIONS, (directory + "/animations/" + clipName + ".ozz").c_str());

	gClip.Initialize(clipPath, &gStickFigureRig);

	// CLIP CONTROLLERS
	//
	// Initialize with the length of the clip they are controlling and an
	// optional external time to set based on their updating
	gClipController.Initialize(gClip.GetDuration(), &this->time);


	// ANIMATIONS
	//
	AnimationDesc animationDesc{};
	animationDesc.mRig = &gStickFigureRig;
	animationDesc.mNumLayers = 1;
	animationDesc.mLayerProperties[0].mClip = &gClip;
	animationDesc.mLayerProperties[0].mClipController = &gClipController;

	gAnimation.Initialize(animationDesc);

	// ANIMATED OBJECTS
	//
	gStickFigureAnimObject.Initialize(&gStickFigureRig, &gAnimation);

}

void OzzObject::removeResources()
{
	removeResource(pGeom);
	removeResource(pTextureDiffuse);

	gStickFigureRig.Destroy();
	gClip.Destroy();
	gAnimation.Destroy();
	gStickFigureAnimObject.Destroy();
}

void OzzObject::update(float deltaTime)
{
	gAnimationUpdateTimer.Reset();

	// Update the animated object for this frame
	if (!gStickFigureAnimObject.Update(deltaTime))
		LOGF(eINFO, "Animation NOT Updating!");

	gStickFigureAnimObject.PoseRig();


	// Record animation update time
	gAnimationUpdateTimer.GetUSec(true);

	// Update uniforms that will be shared between all skeletons
	//gSkeletonBatcher.SetSharedUniforms(Application::projMat * Application::viewMat, vec3(0.0f, 1000.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));

	for (uint i = 0; i < pGeom->mJointCount; ++i) {
		gUniformDataBones.mBoneMatrix[i] = mat4::scale(vec3(100, 100, 100)) * gStickFigureRig.GetJointWorldMat(pGeom->pJointRemaps[i]) *
			mat4::scale(vec3(1, 1, -1)) *
			pGeom->pInverseBindPoses[i];
		//print(gUniformDataBones.mBoneMatrix[i]);
	}
}

void OzzObject::draw(Cmd* cmd)
{
	//gSkeletonBatcher.SetPerInstanceUniforms(Application::gFrameIndex);

	cmdBindVertexBuffer(cmd, 1, &pGeom->pVertexBuffers[0], pGeom->mVertexStrides, (uint64_t*)NULL);
	cmdBindIndexBuffer(cmd, pGeom->pIndexBuffer, pGeom->mIndexType, (uint64_t)NULL);
	cmdDrawIndexed(cmd, pGeom->mIndexCount, 0, 0);
}
