// Per-pixel color data passed through the pixel shader.
Texture2D quadTexture : register(t0);
SamplerState quadSampler : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORDS0;
	float lifeCycle : TEXCOORD1;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 particleColour = quadTexture.Sample(quadSampler, input.uv);
	float4 OriginalparticleColour = quadTexture.Sample(quadSampler, input.uv);
	particleColour = particleColour * (1.0 - smoothstep(0.0, 0.5, length(input.uv - 0.5)));
	return particleColour;// *input.lifeCycle;
}
