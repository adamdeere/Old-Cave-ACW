#pragma once

namespace RenderACW
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT2 canvasSize;
		DirectX::XMFLOAT2 time;
		DirectX::XMFLOAT4 partAmount;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionTex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 tex;
	};

	struct VertexPositionColour
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 colour;
	};

	struct ParticlePosition
	{
		DirectX::XMFLOAT3 pos;
	};
}