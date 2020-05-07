#include "Animator.h"

Animator::Animator(OzzGeode* animatedGeode)
{
	this->directory = ((OzzObject*)animatedGeode->obj)->directory;
	this->addChild(animatedGeode);

	PathHandle skeletonPath = fsCopyPathInResourceDirectory(RD_ANIMATIONS, (directory + "/skeleton.ozz").c_str());
	rig.Initialize(skeletonPath);

	this->clips = &((OzzObject*)animatedGeode->obj)->clips;
	this->inverseBindPoses = &((OzzObject*)animatedGeode->obj)->pGeom->pInverseBindPoses;
	this->jointRemaps = &((OzzObject*)animatedGeode->obj)->pGeom->pJointRemaps;

	AnimationDesc animationDesc{};
	animationDesc.mRig = &rig;
	animationDesc.mNumLayers = (unsigned int)(*clips).size();
	int layer = 0;
	for (auto clipEntry : *clips) {
		float* time = conf_new(float);
		times.emplace(clipEntry.first, time);

		ClipController* cc = conf_new(ClipController);
		cc->Initialize(clipEntry.second->GetDuration(), time);
		clipControllers.emplace(clipEntry.first, cc);

		ClipMask* cm = conf_new(ClipMask);
		cm->Initialize(&rig);
		cm->DisableAllJoints();
		clipMasks.emplace(clipEntry.first, cm);

		animationDesc.mLayerProperties[layer].mClip = clipEntry.second;
		animationDesc.mLayerProperties[layer].mClipController = cc;
		animationDesc.mLayerProperties[layer].mClipMask = cm;
		layer++;
	}
	animation.Initialize(animationDesc);

	animObject.Initialize(&rig, &animation);
}

Animator::Animator(OzzGeode* animatedGeode, mat4 transformation) : Animator(animatedGeode)
{
	this->M = transformation;
}

Animator::~Animator()
{
	rig.Destroy();
	animation.Destroy();
	animObject.Destroy();
	for (auto timeEntry : times) {
		conf_delete(timeEntry.second);
	}
	for (auto ccEntry : clipControllers) {
		conf_delete(ccEntry.second);
	}
	for (auto cmEntry : clipMasks) {
		conf_delete(cmEntry.second);
	}
}

void Animator::SetClip(std::string clipName)
{
	for (auto cmEntry : clipMasks) {
		if (cmEntry.first == clipName) {
			cmEntry.second->EnableAllJoints();
		}
		else {
			cmEntry.second->DisableAllJoints();
		}
	}
}

void Animator::update(float deltaTime)
{
	if (!updated) {
		if (!animObject.Update(deltaTime))
			LOGF(eINFO, "Animation NOT Updating!");

		animObject.PoseRig();

		for (uint i = 0; i < rig.GetNumJoints(); ++i) {
			boneData.mBoneMatrix[i] = mat4::scale(vec3(100, 100, -100)) * rig.GetJointWorldMat((*jointRemaps)[i]) * (*inverseBindPoses)[i];
			//print(boneData.mBoneMatrix[i]);
		}
		updated = true;
	}
	Transform::update(deltaTime);
}

void Animator::updateBoneBuffer(BufferUpdateDesc& desc, OzzObject::UniformDataBones* boneData)
{
	if (!boneData) boneData = &this->boneData;
	Transform::updateBoneBuffer(desc, boneData);
	updated = false;
}
