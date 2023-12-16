#include "stdafx.h"
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
	, myColor{ 0.0f, 0.25f, 0.50f, 1.0f }
	, myVSync(true)
{
}

GraphicsEngine::~GraphicsEngine()
{
}

bool GraphicsEngine::Init(const int aWidth, const int aHeight, HWND& aWindowHandle)
{
	if (!CreateSwapChain(aWindowHandle, aWidth, aHeight))
		return false;

	if (!CreateDepthBuffer(aWidth, aHeight))
		return false;

	if (!CreateBackBuffer())
		return false;

	CreateViewport(aWidth, aHeight);

	if (!CreateFrameBuffer())
		return false;

	if (!CreateSamplerState())
		return false;

	if (!CreateStuffForImGuiImage(aWidth, aHeight))
		return false;

	if (!CreateCameraBuffer())
		return false;

	if (!CreateTimeBuffer())
		return false;

	if (!CreateDirectionalLightBuffer())
		return false;

	LoadSettingsFromJson();

	myCamera->SetResolution(SimpleUtilities::Vector2f{ static_cast<float>(aWidth), static_cast<float>(aHeight) });
	myCamera->SetRotation(SimpleUtilities::Vector3f(50.0f, 0, 0));
	myCamera->SetPosition(SimpleUtilities::Vector3f(0, 15, -12));

	return true;
}

void GraphicsEngine::Update()
{
	{
		FrameBufferData frameBuffer = {};
		frameBuffer.worldToClipMatrix = myCamera->GetWorldToClipMatrix();
		myCameraBuffer->Bind(0);
		myCameraBuffer->Update(sizeof(FrameBufferData), &frameBuffer);
	}

	{
		TimeBufferData timeBuffer = {};
		timeBuffer.time = static_cast<float>(SimplyGlobal::GetTotalTime());
		myTimeBuffer->Bind(2);
		myTimeBuffer->Update(sizeof(TimeBufferData), &timeBuffer);
	}

	{ //Day & Night Cycle
		/*DirectionalLightBufferData directionLightBuffer = {};
		const float cycleDuration = 2;
		const float angularVelocity = 2 * 3.14f / cycleDuration;
		const float elevationAngle = 0.5f * sin(angularVelocity * static_cast<float>(SimplyGlobal::GetTotalTime()) + cycleDuration);

		directionLightBuffer.dir.x = cos(elevationAngle);
		directionLightBuffer.dir.y = sin(elevationAngle);
		directionLightBuffer.dir.z = 0;
		directionLightBuffer.dir.Normalize();

		myDirectionLightBuffer->Bind(3);
		myDirectionLightBuffer->Update(sizeof(DirectionalLightBufferData), &directionLightBuffer);*/

		DirectionalLightBufferData directionLightBuffer = {};

		directionLightBuffer.dir.x = 0.707f;
		directionLightBuffer.dir.y = -0.707f;
		directionLightBuffer.dir.z = 0;
		directionLightBuffer.dir.Normalize();

		myDirectionLightBuffer->Bind(3);
		myDirectionLightBuffer->Update(sizeof(DirectionalLightBufferData), &directionLightBuffer);
	}

	myCamera->Update(SimplyGlobal::GetDeltaTime());
}

void GraphicsEngine::LoadSettingsFromJson()
{
	const std::string filename = SimpleUtilities::GetPath(SIMPLE_SETTINGS_FILENAME);
	std::ifstream file(filename);
	assert(file.is_open() && "Failed To Open File");

	const nlohmann::json json = nlohmann::json::parse(file);
	file.close();

	bool test = json["game_settings"]["vsync"];
	test;

	myVSync = json["game_settings"]["vsync"];
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

	myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());
	//myContext->OMSetRenderTargets(1, myRTV.GetAddressOf(), myDepthBuffer.Get());

	//myContext->ClearRenderTargetView(myRTV.Get(), myColor);
	myContext->ClearRenderTargetView(myBackBuffer.Get(), myColor);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());

	Update();

	return true;
}

void GraphicsEngine::EndFrame()
{
	mySwapChain->Present(myVSync, 0);
}

void GraphicsEngine::SetToBackBuffer()
{
	/*myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myBackBuffer.Get(), myColor);*/
}

void GraphicsEngine::SetVSync(const bool aShouldTurnOn)
{
	myVSync = aShouldTurnOn;
}

ComPtr<ID3D11Device> GraphicsEngine::GetDevice()
{
	return myDevice;
}

ComPtr<ID3D11DeviceContext> GraphicsEngine::GetContext()
{
	return myContext;
}

ComPtr<ID3D11ShaderResourceView> GraphicsEngine::GetShaderResourceView()
{
	return mySRV;
}

std::shared_ptr<Camera> GraphicsEngine::GetCamera()
{
	return myCamera;
}

bool GraphicsEngine::IsVSyncActive() const
{
	return myVSync;
}

bool GraphicsEngine::CreateStuffForImGuiImage(const int aWidth, const int aHeight)
{
	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	texDesc.Width = aWidth;
	texDesc.Height = aHeight;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	HRESULT result = myDevice->CreateTexture2D(&texDesc, nullptr, myTexture.GetAddressOf());
	if (FAILED(result))
		return false;

	result = myDevice->CreateShaderResourceView(myTexture.Get(), nullptr, &mySRV);
	if (FAILED(result))
		return false;

	result = myDevice->CreateRenderTargetView(myTexture.Get(), nullptr, &myRTV);
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
	//swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; //TO-DO: Make this looks good
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
	FrameBufferData cameraBuffer =
	{
		myCamera->GetModelToWorldMatrix().GetFastInverse() * myCamera->GetProjectionMatrix()
	};

	if (!myCameraBuffer->Init(this, sizeof(FrameBufferData), &cameraBuffer))
		return false;

	return true;
}

bool GraphicsEngine::CreateTimeBuffer()
{
	TimeBufferData timeBuffer =
	{
		1.0f
	};

	if (!myTimeBuffer->Init(this, sizeof(TimeBufferData), &timeBuffer))
		return false;

	return true;
}

bool GraphicsEngine::CreateDirectionalLightBuffer()
{
	DirectionalLightBufferData directionLightBuffer =
	{
		SimpleUtilities::Vector3f(0,-1,0)
	};

	if (!myDirectionLightBuffer->Init(this, sizeof(DirectionalLightBufferData), &directionLightBuffer))
		return false;

	return true;
}
