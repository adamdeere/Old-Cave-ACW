cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	float4 TessMode;
	float4 Time;
	float4 CanvasXY;
};

static float3 QuadPos[4] =
{
	float3(-1, 1, 0),
	float3(-1, -1, 0),
	float3(1, 1, 0),
	float3(1, -1, 0)
};

const float2x2 rotate2D = float2x2(1.3623, 1.7531, -1.7131, 1.4623);

float hash(float2 grid)
{
	float h = dot(grid, float2 (127.1, 311.7));
	return frac(sin(h) * 43758.5453123);
}


float Noise(in float2 p)
{
	float2 grid = floor(p);
	float2 f = frac(p);
	float2 uv = f*f*(3.0 - 2.0*f);
	float n1, n2, n3, n4;
	n1 = hash(grid + float2(0.0, 0.0)); n2 = hash(grid + float2(1.0, 0.0));
	n3 = hash(grid + float2(0.0, 1.0)); n4 = hash(grid + float2(1.0, 1.0));
	n1 = lerp(n1, n2, uv.x); n2 = lerp(n3, n4, uv.x);
	n1 = lerp(n1, n2, uv.y);
	return n1;
}

float Terrain(in float2 p)
{
	float2 pos = p*0.05;
	float w = Noise(pos*.25)*0.75 + .15;
	w = 66.0 * w * w;
	float2 dxy = float2(0.0, 0.0);
	float f = .0;
	for (int i = 0; i < 5; i++)
	{
		f += w * Noise(pos);
		w = -w * 0.4;
		pos = mul(rotate2D, pos);
	}
	float ff = Noise(pos*.002);
	f += pow(abs(ff), 5.0)*275. - 5.0;
	return f;
}

struct DS_OUTPUT
{
	float4 Position  : SV_POSITION;
	float2 texCoords : TEXCOORDS0;
	// TODO: change/add other stuff
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float Edges[4] : SV_TessFactor;
	float Inside[2] : SV_InsideTessFactor;
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 4

[domain("quad")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	float pi2 = 6.28;

	float3 vPos1 = (1.0 - UV.y)*QuadPos[0].xyz + UV.y* QuadPos[1].xyz;
	float3 vPos2 = (1.0 - UV.y)*QuadPos[2].xyz + UV.y* QuadPos[3].xyz;

	float3 uvPos = (1.0 - UV.x)*vPos1 + UV.x* vPos2;
	
	float u = Terrain(uvPos.xy * 165);
	float y = Terrain(uvPos.xy * 65);

	//uvPos.x += u;
	uvPos.y -= y;
	//uvPos.z += u;
	
	Output.Position = float4(0.6 *uvPos, 1);

	Output.Position = mul(Output.Position, model);
	Output.Position = mul(Output.Position, view);
	Output.Position = mul(Output.Position, projection);
	Output.texCoords = UV;

	return Output;
}