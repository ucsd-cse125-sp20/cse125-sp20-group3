#include "Animator.h"

Animator::Animator(OzzGeode* animatedGeode)
{
	this->directory = ((OzzObject*)animatedGeode->obj)->directory;
	this->addChild(animatedGeode);

	PathHandle skeletonPath = fsCopyPathInResourceDirectory(RD_ANIMATIONS, (directory + "/skeleton.ozz").c_str());
	gStickFigureRig.Initialize(skeletonPath);

	this->clips = ((OzzObject*)animatedGeode->obj)->clips;
	this->inverseBindPoses = ((OzzObject*)animatedGeode->obj)->pGeom->pInverseBindPoses;
	this->jointRemaps = ((OzzObject*)animatedGeode->obj)->pGeom->pJointRemaps;
}

Animator::~Animator()
{
	gStickFigureRig.Destroy();
	gAnimation.Destroy();
	gStickFigureAnimObject.Destroy();
}

void Animator::SetClip(std::string clipName)
{
	gClipController.Initialize(clips[clipName]->GetDuration(), &this->time);

	AnimationDesc animationDesc{};
	animationDesc.mRig = &gStickFigureRig;
	animationDesc.mNumLayers = 1;
	animationDesc.mLayerProperties[0].mClip = clips[clipName];
	animationDesc.mLayerProperties[0].mClipController = &gClipController;

	gAnimation.Initialize(animationDesc);

	gStickFigureAnimObject.Initialize(&gStickFigureRig, &gAnimation);
}

void Animator::update(float deltaTime)
{
	if (!updated) {
		if (!gStickFigureAnimObject.Update(deltaTime))
			LOGF(eINFO, "Animation NOT Updating!");

		gStickFigureAnimObject.PoseRig();

		for (uint i = 0; i < gStickFigureRig.GetNumJoints(); ++i) {
			boneData.mBoneMatrix[i] = mat4::scale(vec3(100, 100, -100)) * gStickFigureRig.GetJointWorldMat(jointRemaps[i]) * inverseBindPoses[i];
			//print(boneData.mBoneMatrix[i]);
		}
		updated = true;
	}
}

void Animator::updateBoneBuffer(BufferUpdateDesc& desc, OzzObject::UniformDataBones* boneData)
{
	if (!boneData) boneData = &this->boneData;
	Transform::updateBoneBuffer(desc, boneData);
	updated = false;
}
