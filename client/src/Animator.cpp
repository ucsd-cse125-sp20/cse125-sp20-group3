#include "Animator.h"

Animator::Animator(OzzGeode* animatedGeode)
{
	// parent the animated geode node
	this->directory = ((OzzObject*)animatedGeode->obj)->directory;
	this->addChild(animatedGeode);

	// Initiatlize another rig
	PathHandle skeletonPath = fsCopyPathInResourceDirectory(RD_ANIMATIONS, (directory + "/skeleton.ozz").c_str());
	rig.Initialize(skeletonPath);

	// Get a pointer to required data from child
	this->clips = &((OzzObject*)animatedGeode->obj)->clips;
	this->inverseBindPoses = &((OzzObject*)animatedGeode->obj)->pGeom->pInverseBindPoses;
	this->jointRemaps = &((OzzObject*)animatedGeode->obj)->pGeom->pJointRemaps;

	// Build animation
	AnimationDesc animationDesc{};
	animationDesc.mRig = &rig;
	animationDesc.mNumLayers = (unsigned int)(*clips).size();
	int layer = 0;
	for (auto clipEntry : *clips) {
		// Get time reference for external use (probably only for debugging)
		float* time = conf_new(float);
		times.emplace(clipEntry.first, time);

		// Get clip controller
		ClipController* cc = conf_new(ClipController);
		cc->Initialize(clipEntry.second->GetDuration(), time);
		clipControllers.emplace(clipEntry.first, cc);

		// Get clip mask and zero weights
		ClipMask* cm = conf_new(ClipMask);
		cm->Initialize(&rig);
		cm->DisableAllJoints();
		clipMasks.emplace(clipEntry.first, cm);

		//printf("Addresses: %p %p %p\n", (void*)time, (void*)cc, (void*)cm);

		animationDesc.mLayerProperties[layer].mClip = clipEntry.second;
		animationDesc.mLayerProperties[layer].mClipController = cc;
		animationDesc.mLayerProperties[layer].mClipMask = cm;
		layer++;
	}
	animation.Initialize(animationDesc);

	// Initialize animated object wrapper
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
	for (auto d : times) {
		conf_delete(d.second);
	}
	for (auto ccEntry : clipControllers) {
		conf_delete(ccEntry.second);
	}
	for (auto cmEntry : clipMasks) {
		cmEntry.second->Destroy();
		conf_delete(cmEntry.second);
	}
}

void Animator::SetClip(std::string clipName)
{
	currClip = clipName;
	for (auto cmEntry : clipMasks) {
		if (cmEntry.first == clipName) {
			cmEntry.second->EnableAllJoints();
		}
		else {
			cmEntry.second->DisableAllJoints();
		}
	}
}

void Animator::SetLoop(bool state)
{
	if (clipControllers.find(currClip) != clipControllers.end()) {
		clipControllers[currClip]->SetLoop(state);
	}
}

void Animator::SetPlay(bool state)
{
	if (clipControllers.find(currClip) != clipControllers.end()) {
		clipControllers[currClip]->SetPlay(state);
	}
}

void Animator::SetTime(float time)
{
	if (clipControllers.find(currClip) != clipControllers.end()) {
		clipControllers[currClip]->SetTimeRatio(time);
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
