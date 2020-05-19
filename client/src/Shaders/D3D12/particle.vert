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

cbuffer cbRootConstants : register(b2) {
    uint instanceIndex;
};

struct ParticleData {
    float3 position;
    float scale;
    float4 color;
};

StructuredBuffer<float4x4> instanceBuffer : register(t0, UPDATE_FREQ_PER_BATCH);

StructuredBuffer<ParticleData> particleInstanceBuffer : register(t3, UPDATE_FREQ_PER_BATCH);

struct VSOutput
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

VSOutput main(in uint vertexId : SV_VertexID, in uint instanceId : SV_InstanceID)
{
    VSOutput result;
    float x = vertexId / 2;
    float y = vertexId & 1;

    float4x4 toScene = float4x4(1,0,0,0,
                                0,1,0,0,
                                0,0,1,0,
                                0,0,0,1);
    toScene[0][3] = particleInstanceBuffer[instanceId].position.x;
    toScene[1][3] = particleInstanceBuffer[instanceId].position.y;
    toScene[2][3] = particleInstanceBuffer[instanceId].position.z;
    float4x4 toWorld = mul(instanceBuffer[0], toScene);
    float4x4 modelView = mul(view, toWorld);

    float4 vertPos;
    vertPos.x = (x - 0.5f) * particleInstanceBuffer[instanceId].scale + modelView[0][3];
    vertPos.y = (y - 0.5f) * particleInstanceBuffer[instanceId].scale + modelView[1][3];
    vertPos.z = modelView[2][3];
    vertPos.w = 1.0f;

    result.pos = mul(proj, vertPos);

    result.uv = float2(x,1-y);
    result.color = particleInstanceBuffer[instanceId].color;
    return result;
};