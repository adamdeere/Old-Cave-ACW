struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

VS_OUTPUT main(float4 pos : POSITION) : SV_POSITION
{
	VS_OUTPUT Output;

	Output.Position = float4(0.0, 0.0, 0.0, 1.0);

	return Output;
}