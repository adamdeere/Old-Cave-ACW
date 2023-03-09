Texture2D quadTexture : register(t0);
SamplerState quadSampler : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 uv : TEXCOORDS0;

};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 snake = quadTexture.Sample(quadSampler, input.uv);
	
	return snake; 
}