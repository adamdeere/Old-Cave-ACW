// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	float2 canvasSize;
	float2 time;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

struct PixelShaderInput
{
	float4 pos		: SV_POSITION;
	float2 canvasXY : TEXCOORD0;
	float time		: TEXCOORD1;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	//output.pos = float4(sign(input.pos.xy), 0, 1);
	float4 pos = float4(input.pos, 1.0f);
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;
	//output.pos = float4(input.pos, 1.0);

	//float2 canvasSize = float2(800.0, 600.0);
	//output.canvasXY = input.pos.xy * canvasSize;
	output.canvasXY = pos.xy * canvasSize;
	//output.canvasXY = canvasSize;

	output.time = time.x;

	return output;
}
