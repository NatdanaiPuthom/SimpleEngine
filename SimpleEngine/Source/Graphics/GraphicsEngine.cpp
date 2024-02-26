#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/Global.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Game/World.hpp"
#include <External/imgui.h>
#include <External/nlohmann/json.hpp>
#include <fstream>
#include <cmath>

#ifdef _DEBUG
#define REPORT_DX_WARNINGS
#endif

namespace Simple
{
	GraphicsEngine::GraphicsEngine()
		: myClearColor{ 0.0f, 0.0f, 0.0f, 1.0f }
		, myVSync(true)
		, myFPSLevelCap(0)
		, myWaterMoveFactor(0.0f)
	{
	}

	GraphicsEngine::~GraphicsEngine()
	{
	}

	const bool GraphicsEngine::Init(const Math::Vector2ui& aWindowSize, HWND& aWindowHandle)
	{
		myCameraConstantBuffer = std::make_unique<Simple::ConstantBuffer>();
		myTimeConstantBuffer = std::make_unique<Simple::ConstantBuffer>();
		myLightConstantBuffer = std::make_unique<Simple::ConstantBuffer>();
		myBonesConstantBuffer = std::make_unique<Simple::ConstantBuffer>();

		myLightBufferData = std::make_unique<LightBufferData>();
		myViewPort = std::make_shared<D3D11_VIEWPORT>();

		myWaterReflectionRenderTarget = std::make_unique<RenderTarget>();
		myWaterRefractionRenderTarget = std::make_unique<RenderTarget>();
		myImGuiImageRenderTarget = std::make_unique<RenderTarget>();

		myDefaultCamera = std::make_shared<Simple::Camera>();
		myImGuiEngine = std::make_unique<Simple::ImGuiEngine>();

		myDefaultCamera->Init(); //Oppsie, some dependency, need to init here, may fix this someday but not today
		myCurrentCamera = myDefaultCamera;

		myLightBufferData->directionalLightDirection.x = 0.0f;
		myLightBufferData->directionalLightDirection.x = -1.0f;
		myLightBufferData->directionalLightDirection.x = 0.0f;

		CreateSwapChain(aWindowHandle, aWindowSize.x, aWindowSize.y);
		CreateDepthBuffer(aWindowSize.x, aWindowSize.y);
		CreateDepthStencilState();
		CreateBackBuffer();
		CreateViewport(aWindowSize.x, aWindowSize.y);
		CreateFrameBuffer();
		CreateSamplerState();
		CreateCameraBuffer();
		CreateTimeBuffer();
		CreateLightBuffer();
		CreateRasterizerStates();
		CreateRenderTargetForImGuiImage(aWindowSize.x, aWindowSize.y);
		CreateWaterRenderTarget(aWindowSize.x, aWindowSize.y);
		CreateWaterRefractionRenderTarget(aWindowSize.x, aWindowSize.y);
		CreateBonesBuffer();

		LoadSettingsFromJson();
		PreloadTextures();
		PreloadShaders();

		myRenderer = std::make_unique<Drawer::Renderer>();
		myModelFactory = std::make_unique<Simple::ModelFactory>();

		myImGuiEngine->Init();
		myRenderer->Init();
		myModelFactory->Init();

		SetRasterizerState(eRasterizerState::BackfaceCulling);
		myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());

		myCameraConstantBuffer->SetSlot(0);
		myTimeConstantBuffer->SetSlot(2);
		myLightConstantBuffer->SetSlot(3);
		myBonesConstantBuffer->SetSlot(5);

		return true;
	}

	void GraphicsEngine::PrepareFrame()
	{
		{
			FrameBufferData frameBuffer = {};
			frameBuffer.worldToClipMatrix = myCurrentCamera->GetWorldToClipMatrix();
			frameBuffer.cameraPosition = myCurrentCamera->GetPosition();
			frameBuffer.resolution = Global::GetResolution();
			frameBuffer.waterHeight = World::GetWaterHeight();

			myWaterMoveFactor += 0.01f * Global::GetDeltaTime();
			myWaterMoveFactor = std::fmod(myWaterMoveFactor, 1.0f);
			frameBuffer.waterMoveFactor = myWaterMoveFactor;

			myCameraConstantBuffer->Bind(myCameraConstantBuffer->GetSlot());
			myCameraConstantBuffer->Update(sizeof(FrameBufferData), &frameBuffer);
		}

		{
			TimeBufferData timeBuffer = {};
			timeBuffer.totalTime = static_cast<float>(Global::GetTotalTime());
			myTimeConstantBuffer->Bind(myTimeConstantBuffer->GetSlot());
			myTimeConstantBuffer->Update(sizeof(TimeBufferData), &timeBuffer);
		}

		{
			LightBufferData lightBufferData;
			lightBufferData.groundColor = myLightBufferData->groundColor;
			lightBufferData.skyColor = myLightBufferData->skyColor;

			lightBufferData.directionalLightColor = myLightBufferData->directionalLightColor;
			lightBufferData.directionalLightDirection = myLightBufferData->directionalLightDirection;

			lightBufferData.numberOfPointlights = 0;

			{//Test
				myLightBufferData->pointLights[0].position = { -5.0f, 4.0f, 0.0f,1.0f };
				myLightBufferData->pointLights[0].color = { 1.0f, 0.0f, 0.0f,1.0f };
				myLightBufferData->pointLights[0].range = 100.0f;
			}

			for (unsigned int i = 0; i < lightBufferData.numberOfPointlights; i++)
			{
				lightBufferData.pointLights[i].position = myLightBufferData->pointLights[i].position;
				lightBufferData.pointLights[i].color = myLightBufferData->pointLights[i].color;
				lightBufferData.pointLights[i].range = myLightBufferData->pointLights[i].range;
			}

			myLightConstantBuffer->Bind(myLightConstantBuffer->GetSlot());
			myLightConstantBuffer->Update(sizeof(LightBufferData), &lightBufferData);
		}
	}

	void GraphicsEngine::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_FILENAME);
		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		SetVSync(json["game_settings"]["vsync"]);
	}

	void GraphicsEngine::PreloadTextures()
	{
		if (!AddTexture("DefaultTexture.dds", 0))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Cat.dds", 0))
			assert(false && "Failed to add Texture");

		{ //TGA Uppgift
			if (!AddTexture("TGA/Uppgift6/Grass_c.dds", 1))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift6/Rock_c.dds", 2))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift6/Snow_c.dds", 3))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift6/Grass_n.dds", 4))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift6/Rock_n.dds", 5))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift6/Snow_n.dds", 6))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift7/Grass_m.dds", 7))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift7/Rock_m.dds", 8))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift7/Snow_m.dds", 9))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift7/Cubemap.dds", 14))
				assert(false && "Failed to add Texture");

			if (!AddTexture("TGA/Uppgift8/WaveTest.dds", 2))
				assert(false && "Failed to add Texture");
		}
	}

	void GraphicsEngine::PreloadShaders()
	{
		if (!AddShader("DefaultPS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("DefaultColorfulPS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("LinePS.cso", "Line2DVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("LinePS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("DefaultPBRPS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("DefaultPS.cso", "AnimatedModelVS.cso"))
			assert(false && "Failed to add Shader");

		{ //TGA Uppgift
			if (!AddShader("TerrainPS.cso", "DefaultVS.cso"))
				assert(false && "Failed to add Shader");

			if (!AddShader("SkyBoxPS.cso", "SkyBoxVS.cso"))
				assert(false && "Failed to add Shader");

			if (!AddShader("WaterReflectionPS.cso", "PlaneReflectionVS.cso"))
				assert(false && "Failed to add Shader");

			if (!AddShader("DefaultPS.cso", "PlaneReflectionVS.cso"))
				assert(false && "Failed to add Shader");

			if (!AddShader("WaterReflectionPS.cso", "DefaultVS.cso"))
				assert(false && "Failed to add Shader");
		}
	}

	const bool GraphicsEngine::AddTexture(const char* aFileName, const unsigned int aSlot)
	{
		std::shared_ptr<Simple::Texture> texture = std::make_shared<Simple::Texture>();

		if (!texture->LoadDDS(aFileName))
			return false;

		texture->SetSlot(aSlot);

		myLoadedTextures.emplace(aFileName, texture);

		return true;
	}

	const bool GraphicsEngine::AddShader(const char* aPSFile, const char* aVSFile)
	{
		std::string PSKey(aPSFile);
		std::string VSKey(aVSFile);

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

		Impl::SimpleGlobalGraphics::UpdateFPSCounter();
		Impl::SimpleGlobalGraphics::ResetDrawCalls();

		myImGuiEngine->BeginFrame();
		PrepareFrame();

		return true;
	}

	void GraphicsEngine::EndFrame()
	{
		myImGuiEngine->EndFrame();

		mySwapChain->Present(myFPSLevelCap, 0);
	}

	void GraphicsEngine::SetGlobalGraphicsEngineToThis()
	{
		Impl::SimpleGlobalGraphics::SetGraphicsEngine(this);
	}

	void GraphicsEngine::SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen)
	{
		const unsigned int newWidth = aWindowSize.x;
		const unsigned int newHeight = aWindowSize.y;

		DWORD dwStyle = GetWindowLong(Global::GetEngineHWND(), GWL_STYLE);
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

		SetWindowLong(Global::GetEngineHWND(), GWL_STYLE, dwStyle);
		SetWindowPos(Global::GetEngineHWND(), nullptr, 0, 0, width, height, SWP_NOZORDER);

		myBackBuffer->Release();
		const HRESULT result = mySwapChain->ResizeBuffers(2, newWidth, newHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		assert(SUCCEEDED(result) && "Failed to resize buffer");

		ID3D11Texture2D* pBackBuffer = nullptr;

		mySwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		myDevice->CreateRenderTargetView(pBackBuffer, NULL, myBackBuffer.GetAddressOf());

		pBackBuffer->Release();
		myDepthBuffer->Release();

		CreateDepthBuffer(newWidth, newHeight);
		CreateViewport(newWidth, newHeight);
		CreateWaterRenderTarget(newWidth, newHeight);
		CreateRenderTargetForImGuiImage(newWidth, newHeight);
		CreateWaterRefractionRenderTarget(newWidth, newHeight);

		SetRenderTarget(eRenderTarget::Backbuffer);
	}

	void GraphicsEngine::SetRenderTarget(eRenderTarget aRenderTarget)
	{
		ComPtr<ID3D11RenderTargetView> renderTarget;
		ComPtr<ID3D11RasterizerState> rasterizerState;

		switch (aRenderTarget)
		{
		case eRenderTarget::Backbuffer:
			renderTarget = myBackBuffer;
			rasterizerState = myRasterizerState.Get();
			break;
		case eRenderTarget::ImGui:
			renderTarget = myImGuiImageRenderTarget->renderTargetView;
			rasterizerState = myRasterizerState.Get();
			break;
		case eRenderTarget::WaterReflection:
			renderTarget = myWaterReflectionRenderTarget->renderTargetView;
			rasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::FrontFaceCulling)].Get();
			break;
		case eRenderTarget::WaterRefraction:
			renderTarget = myWaterRefractionRenderTarget->renderTargetView;
			rasterizerState = myRasterizerState.Get();
			break;
		default:
			renderTarget = myBackBuffer;
			rasterizerState = myRasterizerState.Get();
			break;
		}

		ID3D11ShaderResourceView* nullSRV = nullptr;
		myContext->PSSetShaderResources(0, 1, &nullSRV);

		myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);
		myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		myContext->OMSetRenderTargets(1, renderTarget.GetAddressOf(), myDepthBuffer.Get());
		myContext->ClearRenderTargetView(renderTarget.Get(), myClearColor);

		myContext->RSSetState(rasterizerState.Get());
	}

	void GraphicsEngine::SetCamera(std::shared_ptr<Simple::Camera> aCamera)
	{
		myCurrentCamera = aCamera;
	}

	void GraphicsEngine::SetToDefaultCamera()
	{
		myCurrentCamera = myDefaultCamera;
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

	void GraphicsEngine::SetDirectionalLightDirection(const Math::Vector3f& aDirection)
	{
		myLightBufferData->directionalLightDirection = aDirection;
	}

	void GraphicsEngine::SetDirectionalLightColor(const Math::Vector4f& aColor)
	{
		myLightBufferData->directionalLightColor = aColor;
	}

	void GraphicsEngine::SetSkyColor(const Math::Vector4f& aColor)
	{
		myLightBufferData->skyColor = aColor;
	}

	void GraphicsEngine::SetGroundColor(const Math::Vector4f& aColor)
	{
		myLightBufferData->groundColor = aColor;
	}

	std::shared_ptr<const Simple::Texture> GraphicsEngine::GetTexture(const char* aFilePath)
	{
		auto it = myLoadedTextures.find(aFilePath);

		if (it != myLoadedTextures.end())
			return it->second;

		return nullptr;
	}

	std::shared_ptr<const Simple::Texture> GraphicsEngine::GetDefaultTexture()
	{
		auto texture = GetTexture("DefaultTexture.dds");

		if (texture != nullptr)
			return texture;

		return nullptr;
	}

	std::shared_ptr<const Simple::Shader> GraphicsEngine::GetDefaultShader()
	{
		auto defaultShader = GetShader("DefaultPS.cso", "DefaultVS.cso");

		if (defaultShader != nullptr)
			return defaultShader;

		return nullptr;
	}

	std::shared_ptr<const Simple::Shader> GraphicsEngine::GetShader(const char* aPSFile, const char* aVSFile)
	{
		auto shader = myLoadedShaders.find({ aPSFile, aVSFile });

		if (shader != myLoadedShaders.end())
			return shader->second;
		else if (shader == myLoadedShaders.end())
		{
			const bool success = AddShader(aPSFile, aVSFile);
			if (success == false)
				assert(false && "Unable to create Shader");
			else
			{
				shader = myLoadedShaders.find({ aPSFile, aVSFile });
				return shader->second;
			}
		}

		return nullptr;
	}

	Drawer::Renderer* GraphicsEngine::GetRenderer()
	{
		return myRenderer.get();
	}

	ModelFactory* GraphicsEngine::GetModelFactory()
	{
		return myModelFactory.get();
	}

	std::shared_ptr<Simple::Camera> GraphicsEngine::GetCurrentCamera()
	{
		return myCurrentCamera;
	}

	std::shared_ptr<Simple::Camera> GraphicsEngine::GetDefaultCamera()
	{
		return myDefaultCamera;
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

	ComPtr<ID3D11ShaderResourceView> GraphicsEngine::GetWaterRefractionShaderResourceView()
	{
		return myWaterRefractionRenderTarget->shaderResourceView;
	}

	Math::Vector4f GraphicsEngine::GetDirectionalLightColor() const
	{
		return myLightBufferData->directionalLightColor;
	}

	Math::Vector3f GraphicsEngine::GetDirectionalLightDirection() const
	{
		return myLightBufferData->directionalLightDirection;
	}

	Math::Vector4f GraphicsEngine::GetSkyColor() const
	{
		return myLightBufferData->skyColor;
	}

	Math::Vector4f GraphicsEngine::GetGroundColor() const
	{
		return myLightBufferData->groundColor;
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

	void GraphicsEngine::CreateRenderTargetForImGuiImage(const int aWidth, const int aHeight)
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
		assert(SUCCEEDED(result) && "Failed to create Texture2D");

		result = myDevice->CreateShaderResourceView(texture, nullptr, &myImGuiImageRenderTarget->shaderResourceView);
		assert(SUCCEEDED(result) && "Failed to create ShaderResourceView");

		result = myDevice->CreateRenderTargetView(texture, nullptr, &myImGuiImageRenderTarget->renderTargetView);
		assert(SUCCEEDED(result) && "Failed to create RenderTargetView");

		texture->Release();
	}

	void GraphicsEngine::CreateWaterRenderTarget(const int aWidth, const int aHeight)
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
		assert(SUCCEEDED(result) && "Failed to create Texture2D");

		result = myDevice->CreateShaderResourceView(texture, nullptr, &myWaterReflectionRenderTarget->shaderResourceView);
		assert(SUCCEEDED(result) && "Failed to create ShaderResourceView");

		result = myDevice->CreateRenderTargetView(texture, nullptr, &myWaterReflectionRenderTarget->renderTargetView);
		assert(SUCCEEDED(result) && "Failed to create RenderTargetView");

		texture->Release();
	}

	void GraphicsEngine::CreateWaterRefractionRenderTarget(const int aWidth, const int aHeight)
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
		assert(SUCCEEDED(result) && "Failed to create Texture2D");

		result = myDevice->CreateShaderResourceView(texture, nullptr, &myWaterRefractionRenderTarget->shaderResourceView);
		assert(SUCCEEDED(result) && "Failed to create ShaderResourceView");

		result = myDevice->CreateRenderTargetView(texture, nullptr, &myWaterRefractionRenderTarget->renderTargetView);
		assert(SUCCEEDED(result) && "Failed to create RenderTargetView");

		texture->Release();
	}

	void GraphicsEngine::CreateRasterizerStates()
	{
		HRESULT result = S_OK;

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.DepthClipEnable = true;

		result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::Wireframe)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: Wireframe");

		rasterizerDesc.CullMode = D3D11_CULL_NONE;

		result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::WireframeNoCulling)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: WireframeNoCulling");

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
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: NoFaceCulling");

		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;

		result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::FrontFaceCulling)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: FrontFaceCulling");

		myRasterizerStates[static_cast<int>(eRasterizerState::BackfaceCulling)] = nullptr;
	}

	void GraphicsEngine::CreateBonesBuffer()
	{
		BonesBufferData bonesBufferData;

		if (!myBonesConstantBuffer->Init(sizeof(BonesBufferData), &bonesBufferData))
			assert(false && "Failed to create BoneConstantBuffer");
	}

	void GraphicsEngine::CreateSwapChain(HWND& aWindowHandle, const int aWidth, const int aHeight)
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

		assert(SUCCEEDED(result) && "Failed to create SwapChain");
	}

	void GraphicsEngine::CreateBackBuffer()
	{
		ID3D11Texture2D* backBufferTexture = nullptr;

		HRESULT result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
		assert(SUCCEEDED(result) && "Failed to get Backbuffer");

		result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myBackBuffer);
		backBufferTexture->Release();
		assert(SUCCEEDED(result) && "Failed to create Backbuffer");
	}

	void GraphicsEngine::CreateFrameBuffer()
	{
		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = sizeof(FrameBufferData);

		HRESULT result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
		assert(SUCCEEDED(result) && "Failed to create Framebuffer");

		bufferDescription.ByteWidth = sizeof(ObjectBufferData);
		result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
		assert(SUCCEEDED(result) && "Failed to create ObjectBuffer");
	}

	void GraphicsEngine::CreateDepthBuffer(const int aWidth, const int aHeight)
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
		assert(SUCCEEDED(result) && "Failed to create DepthStencilTexture");

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		descDSV.Flags = 0;

		result = myDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, myDepthBuffer.GetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create DepthStencilView");
	}

	void GraphicsEngine::CreateDepthStencilState()
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		const HRESULT result = myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStencilState.GetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create DepthStencilState");
	}

	void GraphicsEngine::CreateSamplerState()
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
		assert(SUCCEEDED(result) && "Failed to create SamplerState");
	}

	void GraphicsEngine::CreateCameraBuffer()
	{
		FrameBufferData cameraBuffer;

		cameraBuffer.worldToClipMatrix = Math::Matrix4x4f::GetInverse(myCurrentCamera->GetModelToWorldMatrix()) * myCurrentCamera->GetProjectionMatrix();
		cameraBuffer.cameraPosition = Math::Vector3f{ 0.0f,0.0f,0.0f };

		if (!myCameraConstantBuffer->Init(sizeof(FrameBufferData), &cameraBuffer))
			assert(false && "Failed to create CameraConstantBuffer");
	}

	void GraphicsEngine::CreateTimeBuffer()
	{
		TimeBufferData timeBuffer;

		timeBuffer.totalTime = 0.0f;

		if (!myTimeConstantBuffer->Init(sizeof(TimeBufferData), &timeBuffer))
			assert(false && "Failed to create TimeConstantBuffer");
	}

	void GraphicsEngine::CreateLightBuffer()
	{
		LightBufferData lightBufferData;

		lightBufferData.skyColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		lightBufferData.groundColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

		lightBufferData.directionalLightColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		lightBufferData.directionalLightDirection = Math::Vector3f(0.0f, 0.0f, 0.0f);


		if (!myLightConstantBuffer->Init(sizeof(LightBufferData), &lightBufferData))
			assert(false && "Failed to create LightConstantBuffer");
	}
}