Texture2D txDiffuse : register(t0);
Texture2D lightDiffuse : register(t1);
SamplerState samLinear : register(s0);


// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCORDS;
};

static float2 delta = float2(1.0, 1.0);
static float4  decay = float4(120.0, 100.0f, 100.0f, 10.0f);
static float4 weight = float4(12.0, 111.0f, 10.0f, 10.0f);
static float4 E0 = float4(12.0, 11.0f, 110.0f, 100.0f);
float4 godRays(float2 texCords, float2 pos, Texture2D Texture0)
{
	float2 st = texCords.xy;
	float2 rayDir = normalize(st - pos.xy);
	float4 colour;// = float4(0.0);
	float4 sampleColour;

	for (int i = 0; i < 100; i++)
	{
		st += delta * rayDir;
		sampleColour = Texture0.Sample(samLinear, st.xy);
		sampleColour *= exp(-decay * length(st - pos.xy));
		colour += weight * sampleColour;
	}

	return colour * E0;
}

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 colour = txDiffuse.Sample(samLinear, input.tex);
	float4 rays = godRays(input.tex, input.pos, lightDiffuse);

	return colour + rays;
}
