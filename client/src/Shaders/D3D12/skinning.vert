/*
 * Copyright (c) 2018-2020 The Forge Interactive Inc.
 * 
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/

#define MAX_NUM_BONES 50

cbuffer cbPerPass : register(b0, UPDATE_FREQ_PER_FRAME)
{
	float4x4	projView;
	float4x4	proj;
	float4x4	view;
	float4x4	shadowLightViewProj;
	float4      camPos;
	float4      lightColor[4];
	float4      lightDirection[3];
};

struct InstanceData
{
	float4x4	sceneToWorld;
};

struct BoneData
{
	float4x4	boneMat;
};

cbuffer cbRootConstants : register(b2) {
	uint nodeIndex;
    uint instanceIndex;
	uint modelIndex;
};

StructuredBuffer<InstanceData> instanceBuffer : register(t0, UPDATE_FREQ_PER_BATCH);

StructuredBuffer<BoneData> boneBuffer : register(t2, UPDATE_FREQ_PER_BATCH);

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
	float2 UV : TEXCOORD0;
	float4 BoneWeights : WEIGHTS;
	uint4 BoneIndices : JOINTS;
};

struct PsIn
{
    float3 pos               : POSITION;
	float3 normal	         : NORMAL;
	float2 texCoord          : TEXCOORD0;
    float4 position          : SV_Position;
};

PsIn main(VSInput input)
{
    PsIn result;
	
	float4x4 boneTransform = (boneBuffer[instanceIndex * MAX_NUM_BONES + input.BoneIndices[0]].boneMat) * input.BoneWeights[0];
	boneTransform += (boneBuffer[instanceIndex * MAX_NUM_BONES + input.BoneIndices[1]].boneMat) * input.BoneWeights[1];
	boneTransform += (boneBuffer[instanceIndex * MAX_NUM_BONES + input.BoneIndices[2]].boneMat) * input.BoneWeights[2];
	boneTransform += (boneBuffer[instanceIndex * MAX_NUM_BONES + input.BoneIndices[3]].boneMat) * input.BoneWeights[3];
	
    float4x4 modelMatrix = instanceBuffer[instanceIndex].sceneToWorld;
	modelMatrix[3][0] = 0;
	modelMatrix[3][1] = 0;
	modelMatrix[3][2] = 0;
	modelMatrix[3][3] = 1;
    //modelMatrix = float4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

	float4 skinnedPosition = mul(boneTransform, float4(input.Position, 1.0f));
	//float4 skinnedPosition = float4(input.Position, 1.0f);
	float4 worldPosition = mul(modelMatrix, skinnedPosition);
	//float4 worldPosition = mul(modelMatrix + boneTransform - boneTransform, skinnedPosition);

	result.position = mul(projView, worldPosition);
    result.pos = worldPosition.xyz;
	result.normal = normalize(mul(mul(modelMatrix, boneTransform), float4(input.Normal, 0.0f)).xyz);
	result.texCoord = input.UV;

    return result;
}
