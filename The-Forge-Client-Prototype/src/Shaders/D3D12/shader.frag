
cbuffer uniformBlock : register(b0, UPDATE_FREQ_PER_FRAME) {
	float4x4 projView;
	float4x4 model;
	float4 color;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

struct VSOutput {
	float4 position : SV_Position;
	float2 uv : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET {
	return color;// * tex.Sample(samp, input.uv);
}