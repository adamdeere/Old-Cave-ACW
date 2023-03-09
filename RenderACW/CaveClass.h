#pragma once
#include "..\Common\DeviceResources.h"
class CaveClass
{
public:
	CaveClass();
	~CaveClass();

	void LoadCave();
	
	void RenderCave(ID3D11VertexShader* m_vertexShader, ID3D11PixelShader* m_pixelShader, ID3D11DeviceContext3* context, Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer, ID3D11SamplerState* Sampler, ID3D11ShaderResourceView* bolderTexture);
};

