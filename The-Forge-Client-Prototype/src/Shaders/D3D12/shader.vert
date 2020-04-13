
cbuffer uniformBlock : register(b0, UPDATE_FREQ_PER_FRAME) {
	float4x4 projView;
	float4x4 model;
	float4 color;
};

struct VSInput {
	float4 position : POSITION;
	float4 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VSOutput main(VSInput input) {
	VSOutput output;
	float4x4 pvm = mul(projView, model);
	output.position = mul(pvm, float4(input.position.xyz, 1.0f));
	output.uv = input.uv;

	return output;
}