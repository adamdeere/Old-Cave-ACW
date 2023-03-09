cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	float2 canvasSize;
	float2 time;
};

struct VertexShaderInput
{
	float3 pos : POSITION;
	
};

struct GeoShaderInput
{
	float4 pos		: SV_POSITION;
	float lifeCycle : TEXCOORD1;
};

GeoShaderInput main(VertexShaderInput input) 
{
	GeoShaderInput output;

	float4 inPos = float4(input.pos, 1.0f);

	float param = input.pos.z;

	float speed = 0.7;
	float T = frac(param + speed * time.x);
	float systemHeight = 7.0;
	float radius = 4.5;

	//this will be animated from here
	float3 pos;
	pos.x = radius * T * cos(30.0 * param);
	pos.z = radius * T * sin(30.0 * param);
	pos.y = T * systemHeight;

	float fadeRate = 0.1;
	output.lifeCycle = 1.0 - (T / fadeRate);

	inPos.xyz += pos;

	// Pass the color through without modification.
	output.pos = inPos;
	return output;

	return output;
}