// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	float2 canvasSize;
	float2 time;
};


struct PS_INPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEX;
};
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORDS0;

};

VS_OUTPUT main(PS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(input.Pos, model);


	float t = sin(time.x * output.Pos.y) * output.Pos.y;
	if(output.Pos.y < 0.0f )
	output.Pos.z += sin(output.Pos.y * 1.8f + time.x) * 1.5;
	else if (output.Pos.y > 0.0f)
		output.Pos.x += cos(output.Pos.y * 1.8f + time.x) * 1.5;

	output.Pos.z += cos(output.Pos.x * 0.7f + time.x) * 1.5;

	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);

	output.Tex = input.Tex;
	return output;
}