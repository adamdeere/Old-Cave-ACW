#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace RenderACW;

using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	m_lookController = ref new MoveLookController;
	m_lookController->InitWindow(CoreWindow::GetForCurrentThread());
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
	XMStoreFloat2(&m_constantBufferData.canvasSize, XMVectorSet(m_deviceResources->GetOutputSize().Height, m_deviceResources->GetOutputSize().Width, 0.0, 0.0));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	PositionObject(floorPosition, floorScale, floorX, floorY, floorZ);
	m_constantBufferData.time.x = timer.GetTotalSeconds();
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(0)));
}

void RenderACW::Sample3DSceneRenderer::PositionObject(DirectX::XMFLOAT3 & Position, DirectX::XMFLOAT3 & scale, float & x, float & y, float & z)
{
	
	if (m_lookController->getFoward() == true)
		Position.z -= 0.01f;
			

	if (m_lookController->getBack() == true)
			Position.z += 0.01f;

	if (m_lookController->getUp() == true)
			Position.y += 0.01f;

	if (m_lookController->getDown() == true)
			Position.y -= 0.01f;
	if (m_lookController->getLeft() == true)
			Position.x -= 0.01f;

	if (m_lookController->getRight() == true)
			Position.x += 0.01f;
		//stuuf for the scale

	if (m_lookController->getScaleX() == true)
			scale.x -= 0.01f;

	if (m_lookController->getScaleY() == true)
			scale.y -= 0.01f;

	if (m_lookController->getScaleZ() == true)
			scale.z -= 0.01f;


	if (m_lookController->getOtherScaleX() == true)
		scale.x += 0.01f;

	if (m_lookController->getOtherScaleY() == true)
		scale.y += 0.01f;

	if (m_lookController->getOtherScaleZ() == true)
		scale.z += 0.01f;



		//stuff for the rotate
	if (m_lookController->getRotX() == true)
			x -= 0.01f;


	if (m_lookController->getRotY() == true)
			y -= 0.01f;

	if (m_lookController->getRotZ() == true)
			z -= 0.01f;

	//stuff for the rotate
	if (m_lookController->getOtherRotX() == true)
		x += 0.01f;


	if (m_lookController->getOtherRotY() == true)
		y += 0.01f;

	if (m_lookController->getOtherRotZ() == true)
		z += 0.01f;

}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_constantBuffer.Get(),0,NULL,&m_constantBufferData,0,0,0);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;
	context->IASetVertexBuffers(0,1,m_vertexBuffer.GetAddressOf(),&stride,&offset);

	context->IASetIndexBuffer(m_indexBuffer.Get(),DXGI_FORMAT_R16_UINT, 0);


#pragma region this renders the cave. this works
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->ClearRenderTargetView(renderToEnviromentTargetVIEW, Colors::MidnightBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderToEnviromentTargetVIEW, m_deviceResources->GetDepthStencilView());

	context->VSSetShader(m_EnvriomentVertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

	 //Attach our pixel shader.
	context->PSSetShader(m_EnviromentPixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, &caveTexture);
	context->PSSetShaderResources(1, 1, &lightTexture);
	context->PSSetSamplers(0, 1, &Sampler);
	context->IASetInputLayout(m_EnviromentinputLayout.Get());

	XMMATRIX rotX;
	XMMATRIX rotY;
	XMMATRIX rotZ;


	XMMATRIX translate = XMMatrixTranspose(XMMatrixTranslation(0, 0, 2));
	XMMATRIX scale = XMMatrixTranspose(XMMatrixScaling(1, 1, 3));
	XMMATRIX pos = translate * scale;
	XMStoreFloat4x4(&m_constantBufferData.model, pos);
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	context->DrawIndexed(m_RayindexCount, 0, 0);
#pragma endregion

#pragma region this is where the tessaltion stuff will go

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	context->VSSetShader(m_TessVertexShader.Get(), nullptr, 0);
	context->HSSetShader(m_TessHullShader.Get(), nullptr, 0);
	context->DSSetShader(m_TessDomainShader.Get(), nullptr, 0);
	context->PSSetShader(m_TessPixelShader.Get(), nullptr, 0);
	context->DSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	context->PSSetShaderResources(0, 1, &floorTexture);
	context->PSSetSamplers(0, 1, &Sampler);


	//draw the floor
	translate = XMMatrixTranspose(XMMatrixTranslation(floorPosition.x, floorPosition.y, floorPosition.z));
	scale = XMMatrixTranspose(XMMatrixScaling(floorScale.x, floorScale.y, floorScale.z));
	rotX = XMMatrixTranspose(XMMatrixRotationX(floorX));
	rotY = XMMatrixTranspose(XMMatrixRotationY(floorY));
	rotZ = XMMatrixTranspose(XMMatrixRotationZ(floorZ));
	pos = translate * rotX * rotY * rotZ * scale;
	XMStoreFloat4x4(&m_constantBufferData.model, pos);

	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	context->DrawIndexed(m_indexCount, 0, 0);

	context->PSSetShaderResources(0, 1, &bolderTexture);
	context->PSSetSamplers(0, 1, &Sampler);
	context->DSSetShader(m_SphereDomainShader.Get(), nullptr, 0);

	translate = XMMatrixTranspose(XMMatrixTranslation(spherePosition.x, spherePosition.y, spherePosition.z));
	scale = XMMatrixTranspose(XMMatrixScaling(sphereScale.x, sphereScale.y, sphereScale.z));
	rotX = XMMatrixTranspose(XMMatrixRotationX(sphereX));
	rotY = XMMatrixTranspose(XMMatrixRotationY(sphereY));
	rotZ = XMMatrixTranspose(XMMatrixRotationZ(sphereZ));
	pos = translate * rotX * rotY * rotZ * scale;
	XMStoreFloat4x4(&m_constantBufferData.model, pos);
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	//draw the sphere
	context->DrawIndexed(m_indexCount, 0, 0);

	context->DSSetShader(m_TourusDomainShader.Get(), nullptr, 0);

	translate = XMMatrixTranspose(XMMatrixTranslation(torusPosition.x, torusPosition.y, torusPosition.z));
	scale = XMMatrixTranspose(XMMatrixScaling(torusScale.x, torusScale.y, torusScale.z));
	rotX = XMMatrixTranspose(XMMatrixRotationX(torusX));
	rotY = XMMatrixTranspose(XMMatrixRotationY(torusY));
	rotZ = XMMatrixTranspose(XMMatrixRotationZ(torusZ));
	pos = translate * rotX * rotY * rotZ * scale;
	XMStoreFloat4x4(&m_constantBufferData.model, pos);
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	//draw the torus
	context->DrawIndexed(m_indexCount, 0, 0);

	//draw the water
	context->DSSetShader(m_WaterDomainShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, &waterTexture);
	context->PSSetSamplers(0, 1, &Sampler);
	translate = XMMatrixTranspose(XMMatrixTranslation(0, 0, 0));
	scale = XMMatrixTranspose(XMMatrixScaling(1, 1, 1));
	rotX = XMMatrixTranspose(XMMatrixRotationX(0));
	rotY = XMMatrixTranspose(XMMatrixRotationY(0));
	rotZ = XMMatrixTranspose(XMMatrixRotationZ(0));
	pos = translate * rotX * rotY * rotZ * scale;
	XMStoreFloat4x4(&m_constantBufferData.model, pos);
	XMFLOAT2 t = m_constantBufferData.time;
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	//context->DrawIndexed(m_indexCount, 0, 0);

	context->HSSetShader(nullptr, nullptr, 0);
	context->DSSetShader(nullptr, nullptr, 0);


#pragma endregion

#pragma region this is where the snake goes. could be better but hey ho

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(m_SnakeVertexShader.Get(), nullptr, 0);
	 //Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

	// Attach our pixel shader.
	context->PSSetShader(m_SnakePixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, &snakeTexture);
	context->PSSetSamplers(0, 1, &Sampler);
	context->IASetInputLayout(m_EnviromentinputLayout.Get());

	 translate = XMMatrixTranspose(XMMatrixTranslation(0.3, 0, 0));
	 scale = XMMatrixTranspose(XMMatrixScaling(0.05, 0.05, 0.05));
	 pos = translate * scale;
	XMStoreFloat4x4(&m_constantBufferData.model, pos);
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	context->DrawIndexed(m_RayindexCount, 0, 0);

	translate = XMMatrixTranspose(XMMatrixTranslation(-0.3, 0, 0));
	scale = XMMatrixTranspose(XMMatrixScaling(0.05, 0.05, 0.05));
	pos = translate * scale;
	XMStoreFloat4x4(&m_constantBufferData.model, pos);
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	context->DrawIndexed(m_RayindexCount, 0, 0);


#pragma endregion

#pragma region this is where the particle effects will go
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	 stride = sizeof(ParticlePosition);
	 offset = 0;
	context->IASetVertexBuffers(0, 1, m_PartvertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetInputLayout(m_PartinputLayout.Get());
	context->RSSetState(m_ParticleRasterizerState.Get());
	// Send the constant buffer to the graphics device.


	// Attach our pixel shader.
	context->PSSetShader(m_PartpixelShader.Get(), nullptr, 0);
	// Attach our vertex shader.
	context->VSSetShader(m_PartvertexShader.Get(), nullptr, 0);
	//attahc out geometry shader
	context->GSSetShader(m_PartGeometryShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, &fireTexture);
	context->PSSetSamplers(0, 1, &Sampler);

	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	context->GSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	m_constantBufferData.partAmount = part1;
	
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	// Draw the objects.
	context->OMSetDepthStencilState(DepthStencilStateParticle, 1);
	context->OMSetBlendState(m_blendStateParticle, nullptr, 1);

	context->Draw(particles, 0);

	m_constantBufferData.partAmount = part2;

	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	context->Draw(particles, 0);

	

	m_constantBufferData.partAmount = part3;

	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	context->Draw(particles, 0);

	m_constantBufferData.partAmount = part4;

	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	context->Draw(particles, 0);


	context->OMSetBlendState(nullptr, nullptr, 1);
	context->GSSetShader(nullptr, nullptr, 0);
#pragma endregion

#pragma region this is the ray marching. as good as im going to get it

	//once we get here out cave enviroment is rendered so no we have to change the view over

	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	 //Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	// Each vertex is one instance of the VertexPositionColor struct.
	 stride = sizeof(VertexPositionTex);
	 offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());
	//// Attach our vertex shader.
	context->VSSetShader(m_RayVertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

	// Attach our pixel shader.
	context->PSSetShader(m_RayPixelShader.Get(), nullptr, 0);

	context->PSSetSamplers(0, 1, &Sampler);
	translate = XMMatrixTranspose(XMMatrixTranslation(0, 0, 2));
	scale = XMMatrixTranspose(XMMatrixScaling(1, 1, 1));
	pos = translate * scale;
	XMStoreFloat4x4(&m_constantBufferData.model, pos);
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	context->PSSetShaderResources(0, 1, &caveOutputTexture);
	context->PSSetShaderResources(1, 1, &pillarTexture);
	
	// Draw the objects.
	context->RSSetState(m_RasterState);
	context->DrawIndexed(m_RayindexCount, 0, 0);



#pragma endregion

}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadRayVSTask = DX::ReadDataAsync(L"RayVertexShader.cso");
	auto loadRayPSTask = DX::ReadDataAsync(L"RayPixelShader.cso");

	auto loadEnviromentVSTask = DX::ReadDataAsync(L"CaveVertexShader.cso");
	auto loadEnviromentPSTask = DX::ReadDataAsync(L"CavePixelShader.cso");

	auto loadSnakeVSTask = DX::ReadDataAsync(L"SnakeVertexShader.cso");
	auto loadSnakePSTask = DX::ReadDataAsync(L"SnakePixelShader.cso");

	auto loadTessVSTask = DX::ReadDataAsync(L"TessVertexShader.cso");
	auto loadTessPSTask = DX::ReadDataAsync(L"TessPixelShader.cso");
	auto loadTessHSTask = DX::ReadDataAsync(L"TessHullShader.cso");
	auto loadTessDSTask = DX::ReadDataAsync(L"TessDomainShader.cso");
	auto loadSphereDSTask = DX::ReadDataAsync(L"SphereDomainShader.cso");
	auto loadTourusDSTask = DX::ReadDataAsync(L"TorusDomainShader.cso");
	auto loadWaterDSTask = DX::ReadDataAsync(L"WaterDomainShader.cso");


	auto loadParticleVSTask = DX::ReadDataAsync(L"ParticleVertexShader.cso");
	auto loadParticlePSTask = DX::ReadDataAsync(L"ParticlePixelShader.cso");
	auto loadParticleGSTask = DX::ReadDataAsync(L"ParticleGeometryShader.cso");
	

#pragma region sets us the raster states

	D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, m_ParticleRasterizerState.GetAddressOf()));

	D3D11_RASTERIZER_DESC rasterizerDesc2 = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc2, &m_RasterState));


#pragma endregion

	

#pragma region loads in textures and samplers
	DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"cave.dds", nullptr, &caveTexture));
	DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"snake.dds", nullptr, &snakeTexture));
	DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"fire.dds", nullptr, &fireTexture));
	DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"caveFloor.dds", nullptr, &floorTexture));
	DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"bolder.dds", nullptr, &bolderTexture));
	DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"pillar.dds", nullptr, &pillarTexture));
	DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"lightBeams.dds", nullptr, &waterTexture));
	DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"water.dds", nullptr, &lightTexture));

	
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&sampDesc, &Sampler));
#pragma endregion


#pragma region sets up the target views for multiple render passes

	D3D11_TEXTURE2D_DESC renderableTextureDesc_RenTEX;
	ZeroMemory(&renderableTextureDesc_RenTEX, sizeof(renderableTextureDesc_RenTEX));
	renderableTextureDesc_RenTEX.Width = m_deviceResources->GetOutputSize().Width;
	renderableTextureDesc_RenTEX.Height = m_deviceResources->GetOutputSize().Height;
	renderableTextureDesc_RenTEX.MipLevels = 1;
	renderableTextureDesc_RenTEX.ArraySize = 1;
	renderableTextureDesc_RenTEX.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderableTextureDesc_RenTEX.SampleDesc.Count = 1;
	renderableTextureDesc_RenTEX.Usage = D3D11_USAGE_DEFAULT;
	renderableTextureDesc_RenTEX.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderableTextureDesc_RenTEX.CPUAccessFlags = 0;
	renderableTextureDesc_RenTEX.MiscFlags = 0;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = renderableTextureDesc_RenTEX.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = renderableTextureDesc_RenTEX.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	m_deviceResources->GetD3DDevice()->CreateTexture2D(&renderableTextureDesc_RenTEX, nullptr, &renderToEnviromentTargetTexture);
	m_deviceResources->GetD3DDevice()->CreateRenderTargetView(renderToEnviromentTargetTexture, nullptr, &renderToEnviromentTargetVIEW);
	m_deviceResources->GetD3DDevice()->CreateShaderResourceView(renderToEnviromentTargetTexture, &shaderResourceViewDesc, &caveOutputTexture);

	m_deviceResources->GetD3DDevice()->CreateTexture2D(&renderableTextureDesc_RenTEX, nullptr, &renderRayTargetTexture);
	m_deviceResources->GetD3DDevice()->CreateRenderTargetView(renderRayTargetTexture, nullptr, &renderRayTargetView);
	m_deviceResources->GetD3DDevice()->CreateShaderResourceView(renderRayTargetTexture, &shaderResourceViewDesc, &rayTexture);
	


#pragma endregion

#pragma region sets up a blend state for the particle

	D3D11_BLEND_DESC blendState;
	ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));
	blendState.RenderTarget[0].BlendEnable = TRUE;
	blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendState.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBlendState(&blendState, &m_blendStateParticle));

	D3D11_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&dsDesc, &DepthStencilStateParticle));

#pragma endregion

#pragma region loads in all of the tessaltion shaders
	auto createFloorVSTask = loadTessVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_TessVertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_TessInputLayout
			)
		);
	});

	auto createFloorPSTask = loadTessPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_TessPixelShader
			)
		);
	});

	auto createFloorHSTask = loadTessHSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateHullShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_TessHullShader
			)
		);
	});

	auto createFloorDSTask = loadTessDSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_TessDomainShader
			)
		);
	});
	
	auto createSphereDSTask = loadSphereDSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_SphereDomainShader
			)
		);
	});

	auto createToursDSTask = loadTourusDSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_TourusDomainShader
			)
		);
	});

	auto createWaterDSTask = loadWaterDSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_WaterDomainShader
			)
		);
	});

#pragma endregion

#pragma region loads in all of the envrioment shaders
	// After the vertex shader file is loaded, create the shader and input layout.
	auto createEnviromentVSTask = loadEnviromentVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_EnvriomentVertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCORDS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_EnviromentinputLayout
			)
		);
	});
	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createEnviromentPSTask = loadEnviromentPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_EnviromentPixelShader
			)
		);
	});
#pragma endregion

#pragma region loads in all of the particle shaders

	auto createPartVSTask = loadParticleVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_PartvertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc,ARRAYSIZE(vertexDesc),&fileData[0],fileData.size(),&m_PartinputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPartPSTask = loadParticlePSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_PartpixelShader));


	});

	auto createPartGSTask = loadParticleGSTask.then([this](const std::vector<byte>&fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateGeometryShader(&fileData[0], fileData.size(), nullptr, &m_PartGeometryShader));
	});
#pragma endregion

#pragma region loads in the ray casting shaders

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadRayVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_RayVertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCORDS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
			)
		);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadRayPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_RayPixelShader
			)
		);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
			)
		);
	});

#pragma endregion

#pragma region loads in all of the geometry

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionTex cubeVertices[] =
		{
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices[] =
		{
			3,1,0,
			2,1,3,

			6,4,5,
			7,4,6,

			11,9,8,
			10,9,11,

			14,12,13,
			15,12,14,

			19,17,16,
			18,17,19,

			22,20,21,
			23,20,22
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
			)
		);
	});

	auto CreateRayCubeTask = (createPSTask && createVSTask).then([this]()
	{
		XMFLOAT3 blankColour = XMFLOAT3(0.0f, 0.0f, 0.0f);
		// Load mesh vertices. Each vertex has a position and a color.

		static const VertexPositionColour cubeColourVertices[] =
		{
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), blankColour },
			{ XMFLOAT3(-1.0f, -1.0f,  1.0f), blankColour },
			{ XMFLOAT3(-1.0f,  1.0f, -1.0f), blankColour },
			{ XMFLOAT3(-1.0f,  1.0f,  1.0f), blankColour },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), blankColour },
			{ XMFLOAT3(1.0f, -1.0f,  1.0f), blankColour },
			{ XMFLOAT3(1.0f,  1.0f, -1.0f), blankColour },
			{ XMFLOAT3(1.0f,  1.0f,  1.0f), blankColour },
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeColourVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeColourVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_PointvertexBuffer
			)
		);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short RaycubeIndices[] =
		{
			3,1,0,
			2,1,3,

			6,4,5,
			7,4,6,

			11,9,8,
			10,9,11,

			14,12,13,
			15,12,14,

			19,17,16,
			18,17,19,

			22,20,21,
			23,20,22
		};

		m_RayindexCount = ARRAYSIZE(RaycubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = RaycubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(RaycubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_RayindexBuffer
			)
		);
	});

	auto createPointTask = (createPartPSTask && createPartVSTask).then([this]()
	{
		// Load mesh vertices. Each vertex has a position and a color.

		static  ParticlePosition* PointVertices = new ParticlePosition[particles];

		for (int i = 0; i < particles; i++)
		{
			const float particleZ = (float)i / particles;
			PointVertices[i].pos = { XMFLOAT3(0, 0, particleZ) };
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ParticlePosition) * (1 * particles);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = PointVertices;
		m_deviceResources->GetD3DDevice()->CreateBuffer(&bd, &InitData, &m_PartvertexBuffer);
	});

#pragma endregion

#pragma region loads in the snake shaders

	auto createSnakeVSTask = loadSnakeVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_SnakeVertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEX", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_SnakenputLayout
			)
		);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createSnakePSTask = loadSnakePSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_SnakePixelShader
			)
		);

	});

#pragma endregion


 //Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this] () {
		m_loadingComplete = true;
	});
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_RayVertexShader.Reset();
	m_inputLayout.Reset();
	m_RayPixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}