#include "pch.h"
#include "CaveClass.h"



CaveClass::CaveClass()
{
}


CaveClass::~CaveClass()
{
}

void CaveClass::LoadCave()
{
	
	
}

void CaveClass::RenderCave(ID3D11VertexShader* m_vertexShader, ID3D11PixelShader* m_pixelShader, ID3D11DeviceContext3* context, Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer, ID3D11SamplerState* Sampler, ID3D11ShaderResourceView* bolderTexturer)
{
	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader, nullptr, 0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, &m_constantBuffer, nullptr, nullptr);

	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader, nullptr, 0);
	context->PSSetShaderResources(0, 1, &bolderTexturer);
	context->PSSetSamplers(0, 1, &Sampler);
	// Draw the objects.
	context->DrawIndexed(36, 0, 0);
}
