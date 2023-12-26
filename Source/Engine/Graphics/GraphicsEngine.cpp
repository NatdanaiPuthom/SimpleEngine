#include "Engine/stdafx.h"
#include <External/json.h>
#include <External/imgui.h>

#ifdef _DEBUG
#define REPORT_DX_WARNINGS
#endif

GraphicsEngine::GraphicsEngine()
	: myCamera(std::make_shared<Camera>())
	, myViewPort(std::make_shared<D3D11_VIEWPORT>())
	, myCameraConstantBuffer(std::make_unique<ConstantBuffer>())
	, myTimeConstantBuffer(std::make_unique<ConstantBuffer>())
	, myDirectionLightConstantBuffer(std::make_unique<ConstantBuffer>())
	, myAmbientLightConstantBuffer(std::make_unique<ConstantBuffer>())
	, myDirectionLightData(std::make_unique<DirectionalLightBufferData>())
	, myAmbientLightData(std::make_unique<AmbientLightBufferData>())
	, myWaterReflectionRenderTarget(std::make_unique<RenderTarget>())
	, myImGuiImageRenderTarget(std::make_unique<RenderTarget>())
	, myClearColor{ 0.0f, 0.25f, 0.50f, 1.0f }
	, myVSync(true)
	, myFPSLevelCap(0)
{
	myDirectionLightData->direction.x = 0;
	myDirectionLightData->direction.y = -1;
	myDirectionLightData->direction.z = 0;
}

GraphicsEngine::~GraphicsEngine()
{
}

const bool GraphicsEngine::Init(const SimpleUtilities::Vector2ui& aWindowSize, HWND& aWindowHandle)
{
	if (!CreateSwapChain(aWindowHandle, aWindowSize.x, aWindowSize.y))
		return false;

	if (!CreateDepthBuffer(aWindowSize.x, aWindowSize.y))
		return false;

	if (!CreateDepthStencilState())
		return false;

	if (!CreateBackBuffer())
		return false;

	CreateViewport(aWindowSize.x, aWindowSize.y);

	if (!CreateFrameBuffer())
		return false;

	if (!CreateSamplerState())
		return false;

	if (!CreateCameraBuffer())
		return false;

	if (!CreateTimeBuffer())
		return false;

	if (!CreateDirectionalLightBuffer())
		return false;

	if (!CreateAmbientLightBuffer())
		return false;

	if (!CreateRasterizerStates())
		return false;

	if (!CreateRenderTargetForImGuiImage(aWindowSize.x, aWindowSize.y))
		return false;

	if (!CreateWaterRenderTarget(aWindowSize.x, aWindowSize.y))
		return false;

	LoadSettingsFromJson();
	LoadTextures();
	LoadShaders();
	SetRasterizerState(eRasterizerState::BackfaceCulling);

	myRenderer = std::make_unique<Renderer>();
	myModelFactory = std::make_unique<ModelFactory>();

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
		myCameraConstantBuffer->Bind(0);
		myCameraConstantBuffer->Update(sizeof(FrameBufferData), &frameBuffer);
	}

	{
		TimeBufferData timeBuffer = {};
		timeBuffer.time = static_cast<float>(SimpleGlobal::GetTotalTime());
		myTimeConstantBuffer->Bind(2);
		myTimeConstantBuffer->Update(sizeof(TimeBufferData), &timeBuffer);
	}

	{
		DirectionalLightBufferData directionLightBuffer = {};
		directionLightBuffer.direction = myDirectionLightData->direction;
		directionLightBuffer.direction.Normalize();

		directionLightBuffer.color = myDirectionLightData->color;

		myDirectionLightConstantBuffer->Bind(3);
		myDirectionLightConstantBuffer->Update(sizeof(DirectionalLightBufferData), &directionLightBuffer);
	}

	{
		AmbientLightBufferData ambientLightBuffer = {};
		ambientLightBuffer.skyColor = myAmbientLightData->skyColor;
		ambientLightBuffer.groundColor = myAmbientLightData->groundColor;

		myAmbientLightConstantBuffer->Bind(4);
		myAmbientLightConstantBuffer->Update(sizeof(AmbientLightBufferData), &ambientLightBuffer);
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

	SetVSync(json["game_settings"]["vsync"]);
}

void GraphicsEngine::LoadTextures()
{
	if (!AddTexture("Assets/Textures/DefaultTexture.dds", 0))
		assert(false && "Failed to add Texture");

	if (!AddTexture("Assets/Textures/Cat.dds", 0))
		assert(false && "Failed to add Texture");

	{ //TGA Uppgift
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
	if (!AddShader("Shaders/DefaultPS.cso", "Shaders/DefaultVS.cso"))
		assert(false && "Failed to add Shader");

	{ //TGA Uppgift
		if (!AddShader("Shaders/TerrainPS.cso", "Shaders/TerrainVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("Shaders/SkyBoxPS.cso", "Shaders/SkyBoxVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("Shaders/WaterReflectionPS.cso", "Shaders/DefaultVS.cso"))
			assert(false && "Failed to add Shader");
	}
}

const bool GraphicsEngine::AddTexture(const char* aFilePath, const unsigned int aSlot)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();

	if (!texture->LoadDDS(aFilePath))
		return false;

	texture->SetSlot(aSlot);

	myLoadedTextures.emplace(aFilePath, texture);

	return true;
}

const bool GraphicsEngine::AddShader(const char* aPSFile, const char* aVSFile)
{
	std::string PSKey = aPSFile;
	std::string VSKey = aVSFile;

	std::shared_ptr<Shader> shader = std::make_shared<Shader>();

	if (!shader->Init(myDevice, PSKey, VSKey))
		return false;

	myLoadedShaders.emplace(std::make_pair(PSKey, VSKey), shader);

	return true;
}

const bool GraphicsEngine::BeginFrame()
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
	mySwapChain->Present(myFPSLevelCap, 0);
}

void GraphicsEngine::SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize, const bool aSetFullScreen)
{
	const unsigned int newWidth = aWindowSize.x;
	const unsigned int newHeight = aWindowSize.y;

	DWORD dwStyle = GetWindowLong(SimpleGlobal::GetHWND(), GWL_STYLE);
	if (aSetFullScreen)
	{
		dwStyle &= ~WS_OVERLAPPEDWINDOW;
		dwStyle |= WS_POPUP;
	}
	else {
		dwStyle &= ~WS_POPUP;
		dwStyle |= WS_OVERLAPPEDWINDOW;
	}

	RECT wr = {};
	wr.left = 0;
	wr.right = newWidth + wr.left;
	wr.top = 0;
	wr.bottom = newHeight + wr.top;

	AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

	unsigned int width = wr.right - wr.left;
	unsigned int height = wr.bottom - wr.top;

	if (aSetFullScreen)
	{
		width = newWidth;
		height = newHeight;
	}

	SetWindowLong(SimpleGlobal::GetHWND(), GWL_STYLE, dwStyle);
	SetWindowPos(SimpleGlobal::GetHWND(), nullptr, 0, 0, width, height, SWP_NOZORDER);

	myBackBuffer->Release();
	HRESULT result = mySwapChain->ResizeBuffers(2, newWidth, newHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	if (FAILED(result))
		assert(false && "Failed to re-size Buffers");

	ID3D11Texture2D* pBackBuffer = nullptr;
	mySwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	myDevice->CreateRenderTargetView(pBackBuffer, NULL, myBackBuffer.GetAddressOf());
	pBackBuffer->Release();

	myDepthBuffer->Release();
	if (!CreateDepthBuffer(newWidth, newHeight))
		assert(false && "Failed to re-create DepthBuffer");

	CreateViewport(newWidth, newHeight);

	if (!CreateWaterRenderTarget(newWidth, newHeight))
			assert(false && "Failed to re-create WaterReflectionRenderTarget");

	if (!CreateRenderTargetForImGuiImage(newWidth, newHeight))
			assert(false && "Failed to re-create ImGuiImageRenderTarget");

	SetToBackBuffer();
}

void GraphicsEngine::SetToBackBuffer()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	myContext->PSSetShaderResources(0, 1, &nullSRV);

	myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myBackBuffer.Get(), myClearColor);

	myContext->RSSetState(myRasterizerState.Get());
}

void GraphicsEngine::SetToImGuiBuffer()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	myContext->PSSetShaderResources(0, 1, &nullSRV);

	myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	myContext->OMSetRenderTargets(1, myImGuiImageRenderTarget->renderTargetView.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myImGuiImageRenderTarget->renderTargetView.Get(), myClearColor);

	myContext->RSSetState(myRasterizerState.Get());
}

void GraphicsEngine::SetToWaterReflectionBuffer()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	myContext->PSSetShaderResources(0, 1, &nullSRV);

	myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	myContext->OMSetRenderTargets(1, myWaterReflectionRenderTarget->renderTargetView.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myWaterReflectionRenderTarget->renderTargetView.Get(), myClearColor);

	myContext->RSSetState(myRasterizerStates[static_cast<int>(eRasterizerState::FrontFaceCulling)].Get());
}

void GraphicsEngine::SetRasterizerState(const eRasterizerState aRasterizerState)
{
	switch (aRasterizerState)
	{
	case eRasterizerState::BackfaceCulling:
		myRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::BackfaceCulling)];
		break;
	case eRasterizerState::NoFaceCulling:
		myRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::NoFaceCulling)];
		break;
	case eRasterizerState::Wireframe:
		myRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::Wireframe)];
		break;
	case eRasterizerState::WireframeNoCulling:
		myRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::WireframeNoCulling)];
		break;
	case eRasterizerState::FrontFaceCulling:
		myRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::FrontFaceCulling)];
		break;
	default:
		break;
	}
}

void GraphicsEngine::SetVSync(const bool aShouldTurnOn)
{
	myVSync = aShouldTurnOn;

	if (myVSync)
		myFPSLevelCap = 1;
	else
		myFPSLevelCap = 0;
}

void GraphicsEngine::SetFPSLevelCap(const unsigned int aCapLevel)
{
	if (aCapLevel > 4)
		myFPSLevelCap = 4;

	if (myVSync == false)
		myFPSLevelCap = aCapLevel;
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

std::shared_ptr<Texture> GraphicsEngine::GetTexture(const char* aFilePath)
{
	auto it = myLoadedTextures.find(aFilePath);

	if (it != myLoadedTextures.end())
		return it->second;

	return nullptr;
}

std::shared_ptr<Texture> GraphicsEngine::GetDefaultTexture()
{
	auto texture = GetTexture("Assets/Textures/DefaultTexture.dds");

	if (texture != nullptr)
		return texture;

	return nullptr;
}

std::shared_ptr<Shader> GraphicsEngine::GetDefaultShader()
{
	auto defaultShader = GetShader("Shaders/DefaultPS.cso", "Shaders/DefaultVS.cso");

	if (defaultShader != nullptr)
		return defaultShader;

	return nullptr;
}

std::shared_ptr<Shader> GraphicsEngine::GetShader(const char* aPSFile, const char* aVSFile)
{
	auto shader = myLoadedShaders.find({ aPSFile, aVSFile });

	if (shader != myLoadedShaders.end())
		return shader->second;

	return nullptr;
}

std::shared_ptr<Camera> GraphicsEngine::GetCamera()
{
	return myCamera;
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

ComPtr<ID3D11ShaderResourceView> GraphicsEngine::GetWaterShaderResourceView()
{
	return myWaterReflectionRenderTarget->shaderResourceView;
}

SimpleUtilities::Vector4f GraphicsEngine::GetDirectionalLightColor() const
{
	return myDirectionLightData->color;
}

SimpleUtilities::Vector3f GraphicsEngine::GetDirectionalLightDirection() const
{
	return myDirectionLightData->direction;
}

SimpleUtilities::Vector3f GraphicsEngine::GetSkyColor() const
{
	return myAmbientLightData->skyColor;
}

SimpleUtilities::Vector3f GraphicsEngine::GetGroundColor() const
{
	return myAmbientLightData->groundColor;
}

unsigned int GraphicsEngine::GetFPSLevelCap() const
{
	return myFPSLevelCap;
}

void GraphicsEngine::CreateViewport(const int aWidth, const int aHeight)
{
	std::shared_ptr<D3D11_VIEWPORT> viewport = std::make_shared<D3D11_VIEWPORT>();

	viewport->TopLeftX = 0.0f;
	viewport->TopLeftY = 0.0f;
	viewport->Width = static_cast<float> (aWidth);
	viewport->Height = static_cast<float> (aHeight);
	viewport->MinDepth = 0.0f;
	viewport->MaxDepth = 1.0f;

	myViewPort = viewport;
	myContext->RSSetViewports(1, myViewPort.get());
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

	ID3D11Texture2D* texture = nullptr;

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

bool GraphicsEngine::CreateRasterizerStates()
{
	HRESULT result = S_OK;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = true;

	result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::Wireframe)]);
	if (FAILED(result))
		return false;

	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::WireframeNoCulling)]);
	if (FAILED(result))
		return false;

	rasterizerDesc = {};
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::NoFaceCulling)]);
	if (FAILED(result))
		return false;

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;

	result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::FrontFaceCulling)]);
	if (FAILED(result))
		return false;

	myRasterizerStates[static_cast<int>(eRasterizerState::BackfaceCulling)] = nullptr;

	return true;
}

bool GraphicsEngine::CreateSwapChain(HWND& aWindowHandle, const int aWidth, const int aHeight)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = aWidth;
	swapChainDesc.BufferDesc.Height = aHeight;
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
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = aWidth;
	descDepth.Height = aHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ComPtr<ID3D11Texture2D> pDepthStencil;
	HRESULT result = myDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	if (FAILED(result))
		assert(false && "Failed to create Texture2D");

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	result = myDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, myDepthBuffer.GetAddressOf());
	if (FAILED(result))
		assert(false && "Failed to create DepthStencilView");

	return true;
}

bool GraphicsEngine::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	const HRESULT result = myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStencilState.GetAddressOf());

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

	if (!myCameraConstantBuffer->Init(this, sizeof(FrameBufferData), &cameraBuffer))
		return false;

	return true;
}

bool GraphicsEngine::CreateTimeBuffer()
{
	TimeBufferData timeBuffer;

	timeBuffer.time = 0.0f;

	if (!myTimeConstantBuffer->Init(this, sizeof(TimeBufferData), &timeBuffer))
		return false;

	return true;
}

bool GraphicsEngine::CreateDirectionalLightBuffer()
{
	DirectionalLightBufferData directionLightBuffer;

	directionLightBuffer.direction = SimpleUtilities::Vector3f(0, -1, 0);
	directionLightBuffer.color = SimpleUtilities::Vector4f(0, 0, 0, 0);

	if (!myDirectionLightConstantBuffer->Init(this, sizeof(DirectionalLightBufferData), &directionLightBuffer))
		return false;

	return true;
}

bool GraphicsEngine::CreateAmbientLightBuffer()
{
	AmbientLightBufferData ambientLightBuffer;

	ambientLightBuffer.groundColor = SimpleUtilities::Vector3f(1, 1, 1);
	ambientLightBuffer.skyColor = SimpleUtilities::Vector3f(1, 1, 1);

	if (!myAmbientLightConstantBuffer->Init(this, sizeof(AmbientLightBufferData), &ambientLightBuffer))
		return false;

	return true;
}
