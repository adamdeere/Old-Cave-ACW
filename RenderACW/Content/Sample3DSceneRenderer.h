#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "DDSTextureLoader.h"
#include "CaveClass.h"
#include "MoveLookController.h"

namespace RenderACW
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		
		Microsoft::WRL::ComPtr<ID3D11Buffer>		 m_constantBuffer;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PointvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_RayindexBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;
		uint32	m_RayindexCount;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_RayVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_RayPixelShader;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_EnviromentinputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_EnvriomentVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_EnviromentPixelShader;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_PartinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PartvertexBuffer;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>	 m_PartvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	 m_PartpixelShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_PartGeometryShader;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_ParticleRasterizerState;
		ID3D11ShaderResourceView* fireTexture;

		ID3D11RasterizerState* 	m_RasterState;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_TessInputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_TessVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_TessPixelShader;
		Microsoft::WRL::ComPtr<ID3D11HullShader>	m_TessHullShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>	m_TessDomainShader;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_TessRasterizerState;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>	m_SphereDomainShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>	m_TourusDomainShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>	m_WaterDomainShader;
		ID3D11ShaderResourceView* floorTexture;
		ID3D11ShaderResourceView* bolderTexture;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_SnakenputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_SnakeVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_SnakePixelShader;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
		int particles = 100;
	    DirectX::XMFLOAT2 t;

		ID3D11SamplerState* Sampler;
		ID3D11ShaderResourceView* caveTexture;
		ID3D11ShaderResourceView* snakeTexture;
		CaveClass* cave;

		ID3D11Texture2D* renderToEnviromentTargetTexture;
		ID3D11RenderTargetView* renderToEnviromentTargetVIEW;
		ID3D11ShaderResourceView* caveOutputTexture;

		ID3D11Texture2D* renderRayTargetTexture;
		ID3D11RenderTargetView* renderRayTargetView;
		ID3D11ShaderResourceView* rayTexture;
		ID3D11ShaderResourceView* pillarTexture;
		ID3D11ShaderResourceView* lightTexture;
		ID3D11ShaderResourceView* waterTexture;

		ID3D11BlendState* m_blendStateParticle;
		ID3D11DepthStencilState* DepthStencilStateParticle;

		MoveLookController^ m_lookController;

		DirectX::XMFLOAT3 floorPosition = DirectX::XMFLOAT3(0, -2.2, -2.5);
		DirectX::XMFLOAT3 floorScale = DirectX::XMFLOAT3(6.6, 3.2, -0.2);
		float floorX = -4.9;
		float floorY = 0.0;
		float floorZ = 9.3;

		DirectX::XMFLOAT3 spherePosition = DirectX::XMFLOAT3(-5.1, -4.9, -13);
		DirectX::XMFLOAT3 sphereScale = DirectX::XMFLOAT3(1.6, 1.3, 0.6);
		float sphereX = -1.2;
		float sphereY = -9.3;
		float sphereZ = -1.5;

		DirectX::XMFLOAT3 torusPosition = DirectX::XMFLOAT3(1.6, -4.1, -7.3);
		DirectX::XMFLOAT3 torusScale = DirectX::XMFLOAT3(3, 3, 3);
		float torusX = 0;
		float torusY = 0.0;
		float torusZ = 0;


		DirectX::XMFLOAT4 part1 = DirectX::XMFLOAT4(14.5, -10, -60.5, 0);
		DirectX::XMFLOAT4 part2 = DirectX::XMFLOAT4(-9.5, 0, 10,0);
		DirectX::XMFLOAT4 part3 = DirectX::XMFLOAT4(14.5, 0, -10.5, 0);
		DirectX::XMFLOAT4 part4 = DirectX::XMFLOAT4(-9.5, 0, -10, 0);

		void PositionObject(DirectX::XMFLOAT3 &Position, DirectX::XMFLOAT3 &scale, float &x, float &y, float &z);
		float i = 0;
	};
}

