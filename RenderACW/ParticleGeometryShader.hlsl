cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix View;
	matrix projection;
	float2 canvasSize;
	float2 time;
	float4 partAmount;
};

static const float3 g_positions[4] =
{
	float3(-1, 1, 0),
	float3(-1, -1, 0),
	float3(1, 1, 0),
	float3(1, -1, 0),
};
static const float2 g_texcoords[4] =
{
	float2 (0,1),
	float2 (0,0),
	float2 (1,1),
	float2 (1,0),
};

struct VertexShaderInput
{
	float3 pos : POSITION;
	float lifeCycle : TEXCOORD1;
};

struct GeometryShaderInput
{
	float4 pos : SV_POSITION;
	float lifeCycle : TEXCOORD1;
};
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv: TEXCOORD0;
	float lifeCycle : TEXCOORD1;
};


struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(6)]
void main(point GeometryShaderInput input[1], inout TriangleStream< PixelShaderInput > OutputStream)
{
	PixelShaderInput output = (PixelShaderInput)0;

	float4 vPos = input[0].pos;

	float3 viewLeft = View._11_21_31;
	float3 viewUp = View._12_22_32;
	vPos.xyz = vPos.x * viewLeft + vPos.y * viewUp;
	


	float quadSize = 0.1;
	//triangle 1
	vPos += partAmount;
	vPos = mul(vPos, model);
	vPos = mul(vPos, View);

	//vertex 1.1
	output.pos = vPos + float4(quadSize * g_positions[0], 0.0);
	output.pos = mul(output.pos, projection);
	output.uv = g_texcoords[0];
	OutputStream.Append(output);

	//vertex 2.2
	output.pos = vPos + float4(quadSize * g_positions[1], 0.0);
	output.pos = mul(output.pos, projection);
	output.uv = g_texcoords[1];
	OutputStream.Append(output);

	//vertex 3.3
	output.pos = vPos + float4(quadSize * g_positions[2], 0.0);
	output.pos = mul(output.pos, projection);
	output.uv = g_texcoords[2];
	OutputStream.Append(output);

	//vertex 4.4
	output.pos = vPos + float4(quadSize * g_positions[3], 0.0);
	output.pos = mul(output.pos, projection);
	output.uv = g_texcoords[3];
	OutputStream.Append(output);
	OutputStream.RestartStrip();

	//output.lifeCycle = 1.0 - (T / fadeRate);
}