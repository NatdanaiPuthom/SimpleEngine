#include "Engine/stdafx.h"
#include "Engine/global.hpp"
#include "Graphics/Camera/Camera.hpp"
#include <External/json.h>

#ifdef _DEBUG
#define REPORT_DX_WARNINGS
#endif

GraphicsEngine::GraphicsEngine()
	: myCamera(std::make_shared<Camera>())
	, myCameraBuffer(std::make_unique<ConstantBuffer>())
	, myTimeBuffer(std::make_unique<ConstantBuffer>())
	, myDirectionLightBuffer(std::make_unique<ConstantBuffer>())
	, myAmbientLightBuffer(std::make_unique<ConstantBuffer>())
	, myDirectionLightData(std::make_unique<DirectionalLightBufferData>())
	, myAmbientLightData(std::make_unique<AmbientLightBufferData>())
	, myWaterReflectionRenderTarget(std::make_unique<RenderTarget>())
	, myImGuiImageRenderTarget(std::make_unique<RenderTarget>())
	, myDefaultShader(std::make_shared<Shader>())

	, myColor{ 0.0f, 0.25f, 0.50f, 1.0f }
	, myVSync(true)
{
	myDirectionLightData->direction.x = 0;
	myDirectionLightData->direction.y = -1;
	myDirectionLightData->direction.z = 0;
	myDirectionLightData->color = { 0,0,0, 1 };

	myAmbientLightData->skyColor = { 1,1,1 };
	myAmbientLightData->groundColor = { 1,1,1 };
}

GraphicsEngine::~GraphicsEngine()
{
}

bool GraphicsEngine::Init(const SimpleUtilities::Vector2ui& aWindowSize, HWND& aWindowHandle)
{
	if (!CreateSwapChain(aWindowHandle, aWindowSize.x, aWindowSize.y))
		return false;

	if (!CreateDepthBuffer(aWindowSize.x, aWindowSize.y))
		return false;

	if (!CreateBackBuffer())
		return false;

	CreateViewport(aWindowSize.x, aWindowSize.y);

	if (!CreateFrameBuffer())
		return false;

	if (!CreateSamplerState())
		return false;

	if (!CreateRenderTargetForImGuiImage(aWindowSize.x, aWindowSize.y))
		return false;

	if (!CreateCameraBuffer())
		return false;

	if (!CreateTimeBuffer())
		return false;

	if (!CreateDirectionalLightBuffer())
		return false;

	if (!CreateAmbientLightBuffer())
		return false;

	if (!CreateWaterRenderTarget(aWindowSize.x, aWindowSize.y))
		return false;

	if (!CreateFrontFaceCullingRasterizerState())
		return false;

	LoadSettingsFromJson();
	LoadTextures();
	LoadShaders();

	const SimpleUtilities::Vector2ui resolution = SimpleGlobal::GetResolution();
	myCamera->SetResolution(SimpleUtilities::Vector2f{ static_cast<float>(resolution.x), static_cast<float>(resolution.y) });
	myCamera->SetRotation(SimpleUtilities::Vector3f(50, 0, 0));
	myCamera->SetPosition(SimpleUtilities::Vector3f(10, 15, -12));

	myDirectionLightData->color = SimpleUtilities::Vector4f(1, 1, 1, 1);

	myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());

	return true;
}

void GraphicsEngine::Update()
{
	{
		FrameBufferData frameBuffer = {};
		frameBuffer.worldToClipMatrix = myCamera->GetWorldToClipMatrix();
		frameBuffer.cameraPosition = myCamera->GetPosition();
		frameBuffer.resolution = SimpleGlobal::GetResolution();
		myCameraBuffer->Bind(0);
		myCameraBuffer->Update(sizeof(FrameBufferData), &frameBuffer);
	}

	{
		TimeBufferData timeBuffer = {};
		timeBuffer.time = static_cast<float>(SimpleGlobal::GetTotalTime());
		myTimeBuffer->Bind(2);
		myTimeBuffer->Update(sizeof(TimeBufferData), &timeBuffer);
	}

	{
		DirectionalLightBufferData directionLightBuffer = {};
		directionLightBuffer.direction = myDirectionLightData->direction;
		directionLightBuffer.direction.Normalize();

		directionLightBuffer.color = myDirectionLightData->color;

		myDirectionLightBuffer->Bind(3);
		myDirectionLightBuffer->Update(sizeof(DirectionalLightBufferData), &directionLightBuffer);
	}

	{
		AmbientLightBufferData ambientLightBuffer = {};
		ambientLightBuffer.skyColor = myAmbientLightData->skyColor;
		ambientLightBuffer.groundColor = myAmbientLightData->groundColor;

		myAmbientLightBuffer->Bind(4);
		myAmbientLightBuffer->Update(sizeof(AmbientLightBufferData), &ambientLightBuffer);
	}

	myCamera->Update(SimpleGlobal::GetDeltaTime());
}

void GraphicsEngine::LoadSettingsFromJson()
{
	const std::string filename = SimpleUtilities::GetPath(SIMPLE_SETTINGS_FILENAME);
	std::ifstream file(filename);
	assert(file.is_open() && "Failed To Open File");

	const nlohmann::json json = nlohmann::json::parse(file);
	file.close();

	myVSync = json["game_settings"]["vsync"];
}

void GraphicsEngine::LoadTextures()
{
	if (!AddTexture("Assets/Textures/DefaultTexture.dds", 0))
		assert(false && "Failed to add Texture");

	if (!AddTexture("Assets/Textures/Cat.dds", 0))
		assert(false && "Failed to add Texture");

	{ 	//Test
		if (!AddTexture("Assets/tga/Uppgift6/Grass_c.dds", 1))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets/tga/Uppgift6/Rock_c.dds", 2))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets/tga/Uppgift6/Snow_c.dds", 3))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets/tga/Uppgift6/Grass_n.dds", 4))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets/tga/Uppgift6/Rock_n.dds", 5))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets/tga/Uppgift6/Snow_n.dds", 6))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets/tga/Uppgift7/Grass_m.dds", 7))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets/tga/Uppgift7/Rock_m.dds", 8))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets/tga/Uppgift7/Snow_m.dds", 9))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets/tga/Uppgift7/cubemap.dds", 14))
			assert(false && "Failed to add Texture");
	}
}

void GraphicsEngine::LoadShaders()
{
	if (!myDefaultShader->Init(myDevice, "Shaders/DefaultPS.cso", "Shaders/DefaultVS.cso"))
		assert(false && "Failed to init Shader");
}

bool GraphicsEngine::AddTexture(const char* aFilePath, const unsigned int aSlot)
{
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	if (!texture->LoadDDS(aFilePath))
		return false;

	texture->SetSlot(aSlot);

	myLoadedTextures.emplace(aFilePath, std::move(texture));

	return true;
}

bool GraphicsEngine::BeginFrame()
{
	MSG msg = { 0 };

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			return false;
	}

	Update();

	return true;
}

void GraphicsEngine::EndFrame()
{
	mySwapChain->Present(myVSync, 0);
}

void GraphicsEngine::SetToBackBuffer()
{
	myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myBackBuffer.Get(), myColor);

	myContext->RSSetState(nullptr);
}

void GraphicsEngine::SetToImGuiBuffer()
{
	myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	myContext->OMSetRenderTargets(1, myImGuiImageRenderTarget->renderTargetView.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myImGuiImageRenderTarget->renderTargetView.Get(), myColor);

	myContext->RSSetState(nullptr);
}

void GraphicsEngine::SetToWaterReflectionBuffer()
{
	myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	myContext->OMSetRenderTargets(1, myWaterReflectionRenderTarget->renderTargetView.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myWaterReflectionRenderTarget->renderTargetView.Get(), myColor);

	myContext->RSSetState(myFrontFaceCullingRasterizerState.Get());
}

void GraphicsEngine::SetVSync(const bool aShouldTurnOn)
{
	myVSync = aShouldTurnOn;
}

void GraphicsEngine::SetDirectionalLightDirection(const SimpleUtilities::Vector3f& aDirection)
{
	myDirectionLightData->direction = aDirection;
}

void GraphicsEngine::SetDirectionalLightColor(const SimpleUtilities::Vector4f& aColor)
{
	myDirectionLightData->color = aColor;
}

void GraphicsEngine::SetSkyColor(const SimpleUtilities::Vector3f& aColor)
{
	myAmbientLightData->skyColor = aColor;
}

void GraphicsEngine::SetGroundColor(const SimpleUtilities::Vector3f& aColor)
{
	myAmbientLightData->groundColor = aColor;
}

Texture* GraphicsEngine::GetTexture(const char* aFilePath)
{
	auto it = myLoadedTextures.find(aFilePath);

	if (it != myLoadedTextures.end())
		return it->second.get();

	return nullptr;
}

std::shared_ptr<Shader> GraphicsEngine::GetDefaultShader()
{
	return myDefaultShader;
}

ComPtr<ID3D11Device> GraphicsEngine::GetDevice()
{
	return myDevice;
}

ComPtr<ID3D11DeviceContext> GraphicsEngine::GetContext()
{
	return myContext;
}

ComPtr<ID3D11ShaderResourceView> GraphicsEngine::GetImGuiShaderResourceView()
{
	return myImGuiImageRenderTarget->shaderResourceView;
}

std::shared_ptr<Camera> GraphicsEngine::GetCamera()
{
	return myCamera;
}

SimpleUtilities::Vector3f GraphicsEngine::GetDirectionalLightDirection() const
{
	return myDirectionLightData->direction;
}

SimpleUtilities::Vector4f GraphicsEngine::GetDirectionalLightColor() const
{
	return myDirectionLightData->color;
}

SimpleUtilities::Vector3f GraphicsEngine::GetSkyColor() const
{
	return myAmbientLightData->skyColor;
}

SimpleUtilities::Vector3f GraphicsEngine::GetGroundColor() const
{
	return myAmbientLightData->groundColor;
}

bool GraphicsEngine::IsVSyncActive() const
{
	return myVSync;
}

bool GraphicsEngine::CreateRenderTargetForImGuiImage(const int aWidth, const int aHeight)
{
	D3D11_TEXTURE2D_DESC desc = { 0 };

	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;

	HRESULT result = myDevice->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateShaderResourceView(texture, nullptr, &myImGuiImageRenderTarget->shaderResourceView);
	if (FAILED(result))
		return false;

	result = myDevice->CreateRenderTargetView(texture, nullptr, &myImGuiImageRenderTarget->renderTargetView);
	if (FAILED(result))
		return false;

	texture->Release();

	return true;
}

bool GraphicsEngine::CreateWaterRenderTarget(const int aWidth, const int aHeight)
{
	D3D11_TEXTURE2D_DESC desc = { 0 };

	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;

	HRESULT result = myDevice->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateShaderResourceView(texture, nullptr, &myWaterReflectionRenderTarget->shaderResourceView);
	if (FAILED(result))
		return false;

	result = myDevice->CreateRenderTargetView(texture, nullptr, &myWaterReflectionRenderTarget->renderTargetView);
	if (FAILED(result))
		return false;

	texture->Release();

	return true;
}

bool GraphicsEngine::CreateFrontFaceCullingRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};

	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;

	const HRESULT result = myDevice->CreateRasterizerState(&rasterizerDesc, &myFrontFaceCullingRasterizerState);
	if (FAILED(result))
		return false;

	return true;
}

void GraphicsEngine::CreateViewport(const int aWidth, const int aHeight)
{
	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float> (aWidth);
	viewport.Height = static_cast<float> (aHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	myContext->RSSetViewports(1, &viewport);
}

bool GraphicsEngine::CreateSwapChain(HWND& aWindowHandle, const int aWidth, const int aHeight)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = aWidth;
	swapChainDesc.BufferDesc.Height = aHeight;
	//swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; //TO-DO: I don't know but everything is extra bright
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;

	UINT creationFlags = 0;
#if defined (REPORT_DX_WARNINGS)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myContext
	);

	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::CreateBackBuffer()
{
	ID3D11Texture2D* backBufferTexture = nullptr;

	HRESULT result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myBackBuffer);
	backBufferTexture->Release();

	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::CreateFrameBuffer()
{
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.ByteWidth = sizeof(FrameBufferData);

	HRESULT result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	if (FAILED(result))
		return false;

	bufferDescription.ByteWidth = sizeof(ObjectBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::CreateDepthBuffer(const int aWidth, const int aHeight)
{
	ComPtr<ID3D11Texture2D> depthBufferTexture;
	D3D11_TEXTURE2D_DESC depthBufferDesc = { 0 };
	depthBufferDesc.Height = aHeight;
	depthBufferDesc.Width = aWidth;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	HRESULT result = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthBufferTexture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateDepthStencilView(depthBufferTexture.Get(), nullptr, &myDepthBuffer);
	if (FAILED(result))
		return false;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	result = myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStencilState.GetAddressOf());
	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::CreateSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	const HRESULT result = myDevice->CreateSamplerState(&samplerDesc, &mySamplerState);
	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::CreateCameraBuffer()
{
	FrameBufferData cameraBuffer;

	cameraBuffer.worldToClipMatrix = myCamera->GetModelToWorldMatrix().GetFastInverse() * myCamera->GetProjectionMatrix();
	cameraBuffer.cameraPosition = SimpleUtilities::Vector3f{ 0.0f,0.0f,0.0f };

	if (!myCameraBuffer->Init(this, sizeof(FrameBufferData), &cameraBuffer))
		return false;

	return true;
}

bool GraphicsEngine::CreateTimeBuffer()
{
	TimeBufferData timeBuffer;

	timeBuffer.time = 0.0f;

	if (!myTimeBuffer->Init(this, sizeof(TimeBufferData), &timeBuffer))
		return false;

	return true;
}

bool GraphicsEngine::CreateDirectionalLightBuffer()
{
	DirectionalLightBufferData directionLightBuffer;

	directionLightBuffer.direction = SimpleUtilities::Vector3f(0, -1, 0);
	directionLightBuffer.color = SimpleUtilities::Vector4f(0, 0, 0, 0);

	if (!myDirectionLightBuffer->Init(this, sizeof(DirectionalLightBufferData), &directionLightBuffer))
		return false;

	return true;
}

bool GraphicsEngine::CreateAmbientLightBuffer()
{
	AmbientLightBufferData ambientLightBuffer;

	ambientLightBuffer.groundColor = SimpleUtilities::Vector3f(1, 1, 1);
	ambientLightBuffer.skyColor = SimpleUtilities::Vector3f(1, 1, 1);

	if (!myAmbientLightBuffer->Init(this, sizeof(AmbientLightBufferData), &ambientLightBuffer))
		return false;

	return true;
}
