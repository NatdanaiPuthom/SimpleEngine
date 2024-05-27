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

namespace Graphics
{
	GraphicsEngine::GraphicsEngine()
		: myClearColor{ 0.0f, 0.0f, 0.0f, 1.0f }
		, myVSync(true)
		, myFPSLevelCap(0)
	{
	}

	GraphicsEngine::~GraphicsEngine()
	{
	}

	const bool GraphicsEngine::Init(HWND& aWindowHandle, const Math::Vector2ui& aWindowSize)
	{
		myCameraConstantBuffer = std::make_unique<ConstantBuffer>();
		myTimeConstantBuffer = std::make_unique<ConstantBuffer>();
		myLightConstantBuffer = std::make_unique<ConstantBuffer>();
		myJointsConstantBuffer = std::make_unique<ConstantBuffer>();

		myImGuiEngine = std::make_unique<Simple::ImGuiEngine>();
		myLightBufferData = std::make_unique<LightBufferData>();
		myViewPort = std::make_shared<D3D11_VIEWPORT>();

		myEditorCamera = std::make_shared<Graphics::Camera>();
		myShadowCamera = std::make_shared<Graphics::Camera>();

		myRenderer = std::make_unique<Drawer::Renderer>();
		myModelFactory = std::make_unique<ModelFactory>();

		LoadSettingsFromJson();

		CreateSwapChain(aWindowHandle, aWindowSize);
		CreateViewport(aWindowSize);

		CreateBackBuffer();
		CreateGBuffer(aWindowSize);
		CreateDeferredBuffer(aWindowSize);

		CreateDepthBuffer(aWindowSize);
		CreateDepthStencilState();
		CreateRasterizerStates();
		CreateSamplerState();

		CreateCameraBuffer();
		CreateTimeBuffer();
		CreateLightBuffer();
		CreateBonesBuffer();

		PreloadTextures();
		PreloadShaders();

		myEditorCamera->Init();
		myImGuiEngine->Init();
		myRenderer->Init();
		myModelFactory->Init();

		SetRasterizerState(eRasterizerState::BackfaceCulling);

		myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());
		myContext->RSSetViewports(1, myViewPort.get());
		myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);

		myCameraConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Camera);
		myTimeConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Time);
		myLightConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Light);
		myJointsConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Joints);

		myLightBufferData->directionalLightDirection.x = 0.0f;
		myLightBufferData->directionalLightDirection.y = -1.0f;
		myLightBufferData->directionalLightDirection.z = 0.0f;

		myCurrentCamera = myEditorCamera;

		return true;
	}

	void GraphicsEngine::PrepareFrame()
	{
		UpdateCameraBuffer();

		{
			TimeBufferData timeBuffer = {};
			timeBuffer.totalTime = static_cast<float>(Global::GetTotalTime());
			timeBuffer.deltaTime = static_cast<float>(Global::GetDeltaTime());
			myTimeConstantBuffer->Bind(myTimeConstantBuffer->GetSlot());
			myTimeConstantBuffer->Update(sizeof(TimeBufferData), &timeBuffer);
		}

		{
			LightBufferData lightBufferData;

			lightBufferData.directionalLightWorldToProjectionMatrix = Math::Matrix4x4f::GetFastInverse(myShadowCamera->GetMatrix()) * myShadowCamera->GetProjectionMatrix();
			lightBufferData.ambientLightColorAndIntensity = myLightBufferData->ambientLightColorAndIntensity;
			lightBufferData.directionalLightColorAndIntensity = myLightBufferData->directionalLightColorAndIntensity;
			lightBufferData.directionalLightDirection = myLightBufferData->directionalLightDirection;

			myLightConstantBuffer->Bind(myLightConstantBuffer->GetSlot());
			myLightConstantBuffer->Update(sizeof(LightBufferData), &lightBufferData);
		}
	}

	void GraphicsEngine::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		SetVSync(json["game_settings"]["vsync"]);
	}

	void GraphicsEngine::PreloadTextures()
	{
		if (!AddTexture("Assets\\Textures\\DefaultTexture.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cat.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Hamster.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cat-scared.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\CloudCubeMap_1024.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\NightStarsCubeMap.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\CloudAnime.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\AutumnForest.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");
	}

	void GraphicsEngine::PreloadShaders()
	{
		if (!AddShader("DefaultPS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("DefaultPS.cso", "AnimatedModelVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("DefaultPBRPS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("LinePS.cso", "Line2DVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("SkyBoxPS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("GBufferPS.cso", "GBufferVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("DeferredPS.cso", "FullScreenVS.cso"))
			assert(false && "Failed to add Shader");
	}

	const bool GraphicsEngine::AddTexture(const char* aFileName, const unsigned int aSlot)
	{
		auto it = myLoadedTextures.find(aFileName);

		if (it != myLoadedTextures.end())
		{
			return false;
		}

		std::shared_ptr<Texture> texture = std::make_shared<Texture>();

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

	bool GraphicsEngine::BeginFrame()
	{
		MSG msg = { 0 };

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				Global::SetGameShouldClose(true);
				return false;
			}
		}

		if (Impl::SimpleGlobalGraphics::GetShouldResizeWindow())
		{
			Impl::SimpleGlobalGraphics::ResizeWindow();
		}

		Impl::SimpleGlobalGraphics::UpdateFPSCounter();
		Impl::SimpleGlobalGraphics::ResetDrawCalls();

		PROFILER_BEGIN("ImGui BeginFrame");
		myImGuiEngine->BeginFrame();
		PROFILER_END();

		PROFILER_BEGIN("Prepare Frame");
		PrepareFrame();
		PROFILER_END();

		return true;
	}

	void GraphicsEngine::EndFrame()
	{
		PROFILER_BEGIN("ImGui Endframe");
		myImGuiEngine->EndFrame();
		PROFILER_END();

		PROFILER_BEGIN("Present frame");
		mySwapChain->Present(myFPSLevelCap, 0);
		PROFILER_END();
	}

	void GraphicsEngine::RenderDeferredFromGBuffer()
	{
		static constexpr size_t gBufferCount = 5;

		std::vector<Graphics::RenderTarget>& gBuffers = myRenderTargets[static_cast<size_t>(eRenderTargetType::GBuffer)];

		ID3D11ShaderResourceView* shaderResources[gBufferCount] = {};

		for (size_t i = 0; i < gBuffers.size(); ++i)
		{
			shaderResources[i] = gBuffers[i].shaderResourceView.Get();
		}

		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, gBufferCount, shaderResources);

		std::shared_ptr<const Shader> shader = GetShader(eShaderType::Deferred);
		shader->BindThisShader(myContext.Get());

		myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myContext->IASetInputLayout(nullptr);
		myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		myContext->GSSetShader(nullptr, nullptr, 0);
		myContext->Draw(3, 0);

		ID3D11ShaderResourceView* nullSRVs[gBufferCount] = { NULL };
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, gBufferCount, nullSRVs);
	}

	void GraphicsEngine::UnbindAllRenderTargets()
	{
		static constexpr size_t maxRenderTargetSupportedByDX11 = 8;
		ID3D11RenderTargetView* nullViews[maxRenderTargetSupportedByDX11] = { nullptr };
		myContext->OMSetRenderTargets(maxRenderTargetSupportedByDX11, nullViews, nullptr);
	}

	void GraphicsEngine::UpdateCameraBuffer()
	{
		CameraBufferData frameBuffer = {};
		frameBuffer.worldToClipMatrix = myCurrentCamera->GetWorldToClipMatrix();
		frameBuffer.cameraPosition = myCurrentCamera->GetPosition();
		frameBuffer.resolution = Global::GetResolution();

		myCameraConstantBuffer->Bind(myCameraConstantBuffer->GetSlot());
		myCameraConstantBuffer->Update(sizeof(CameraBufferData), &frameBuffer);
	}

	void GraphicsEngine::SetGlobalGraphicsEngineToThis()
	{
		Impl::SimpleGlobalGraphics::SetGraphicsEngine(this);
	}

	void GraphicsEngine::SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen)
	{
		Math::Vector2ui newWindowSize = aWindowSize;

		DWORD dwStyle = GetWindowLong(Global::GetEngineHWND(), GWL_STYLE);

		if (aSetFullScreen)
		{
			dwStyle &= ~WS_OVERLAPPEDWINDOW;
			dwStyle |= WS_POPUP;
		}
		else
		{
			dwStyle = Global::GetOriginalWindowStyle();
		}

		RECT wr = {};
		wr.left = 0;
		wr.right = newWindowSize.x + wr.left;
		wr.top = 0;
		wr.bottom = newWindowSize.y + wr.top;

		AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		unsigned int width = wr.right - wr.left;
		unsigned int height = wr.bottom - wr.top;

		if (aSetFullScreen)
		{
			HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);

			MONITORINFOEX monitorInfo = { sizeof(MONITORINFOEX) };
			GetMonitorInfo(hMonitor, &monitorInfo);

			width = static_cast<unsigned int>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);
			height = static_cast<unsigned int>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);

			newWindowSize.x = width;
			newWindowSize.y = height;
		}

		SetWindowLong(Global::GetEngineHWND(), GWL_STYLE, dwStyle);
		SetWindowPos(Global::GetEngineHWND(), nullptr, 0, 0, width, height, SWP_NOZORDER);

		ID3D11RenderTargetView* backBuffer = myRenderTargets[static_cast<size_t>(eRenderTargetType::Backbuffer)][0].renderTargetView.Get();
		backBuffer->Release();
		myDepthBuffer->Release();

		const HRESULT result = mySwapChain->ResizeBuffers(0, newWindowSize.x, newWindowSize.y, DXGI_FORMAT_UNKNOWN, 0);
		assert(SUCCEEDED(result) && "Failed to resize buffer");

		CreateBackBuffer();
		CreateDepthBuffer(newWindowSize);
		CreateGBuffer(newWindowSize);
		CreateDeferredBuffer(newWindowSize);

		CreateViewport(newWindowSize);

		myContext->RSSetViewports(1, myViewPort.get());
	}

	void GraphicsEngine::SetRenderTarget(eRenderTargetType aRenderTargetType, ID3D11DepthStencilView* aDepthBuffer)
	{
		myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		UnbindAllRenderTargets();

		const std::vector<RenderTarget>& originalRenderTargets = myRenderTargets[static_cast<size_t>(aRenderTargetType)];
		const size_t count = originalRenderTargets.size();

		std::vector<ID3D11RenderTargetView*> renderTargetsPointer(count);

		for (size_t i = 0; i < count; ++i)
		{
			renderTargetsPointer[i] = originalRenderTargets[i].renderTargetView.Get();
			myContext->ClearRenderTargetView(renderTargetsPointer[i], &myClearColor[0]);
		}

		myContext->OMSetRenderTargets(static_cast<unsigned int>(count), &renderTargetsPointer[0], aDepthBuffer);
	}

	void GraphicsEngine::SetCamera(std::shared_ptr<Graphics::Camera> aCamera)
	{
		myCurrentCamera = aCamera;
		UpdateCameraBuffer();
	}

	void GraphicsEngine::SetToDefaultCamera()
	{
		myCurrentCamera = myEditorCamera;
	}

	std::vector<RenderTarget>& GraphicsEngine::GetRenderTargets(const eRenderTargetType aRenderTargetType)
	{
		return myRenderTargets[static_cast<size_t>(aRenderTargetType)];
	}

	void GraphicsEngine::SetRasterizerState(const eRasterizerState aRasterizerState)
	{
		switch (aRasterizerState)
		{
		case eRasterizerState::BackfaceCulling:
			myCurrentRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::BackfaceCulling)];
			break;
		case eRasterizerState::NoFaceCulling:
			myCurrentRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::NoFaceCulling)];
			break;
		case eRasterizerState::Wireframe:
			myCurrentRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::Wireframe)];
			break;
		case eRasterizerState::WireframeNoCulling:
			myCurrentRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::WireframeNoCulling)];
			break;
		case eRasterizerState::FrontFaceCulling:
			myCurrentRasterizerState = myRasterizerStates[static_cast<int>(eRasterizerState::FrontFaceCulling)];
			break;
		default:
			break;
		}

		myContext->RSSetState(myCurrentRasterizerState.Get());
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
		myLightBufferData->directionalLightColorAndIntensity = aColor;
	}

	void GraphicsEngine::SetAmbientLightColorAndIntensity(const Math::Vector4f& aColorAndIntensity)
	{
		myLightBufferData->ambientLightColorAndIntensity = aColorAndIntensity;
	}

	std::shared_ptr<const Texture> GraphicsEngine::GetTexture(const char* aFilePath)
	{
		auto it = myLoadedTextures.find(aFilePath);

		if (it != myLoadedTextures.end())
		{
			return it->second;
		}
		else
		{
			unsigned int slot = 0;

			if (SimpleUtilities::FindSuffix(aFilePath, "_D"))
			{
				slot = Graphics::Global_Slot_Albedo;
			}
			else if (SimpleUtilities::FindSuffix(aFilePath, "_M"))
			{
				slot = Graphics::Global_Slot_Material;
			}
			else if (SimpleUtilities::FindSuffix(aFilePath, "_N"))
			{
				slot = Graphics::Global_Slot_Normal;
			}

			if (AddTexture(aFilePath, slot) == true)
			{
				it = myLoadedTextures.find(aFilePath);

				if (it != myLoadedTextures.end())
				{
					return it->second;
				}
			}
		}

		return nullptr;
	}

	std::shared_ptr<const Texture> GraphicsEngine::GetTexture(const eTextureType aTextureType)
	{
		std::shared_ptr<const Texture> texture = nullptr;

		switch (aTextureType)
		{
		case eTextureType::Default:
			texture = GetTexture("Assets\\Textures\\DefaultTexture.dds");
			break;
		case eTextureType::SkyBox_DayCloud:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\CloudCubeMap_1024.dds");
			break;
		case eTextureType::SkyBox_NightStar:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\NightStarsCubeMap.dds");
			break;
		case eTextureType::SkyBox_DayGrassland:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\CloudAnime.dds");
			break;
		case eTextureType::SkyBox_AutumnForest:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\AutumnForest.dds");
			break;
		}

		return texture;
	}

	std::shared_ptr<const Shader> GraphicsEngine::GetShader(const char* aPSFile, const char* aVSFile)
	{
		auto shader = myLoadedShaders.find({ aPSFile, aVSFile });

		if (shader != myLoadedShaders.end())
		{
			return shader->second;
		}
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

	std::shared_ptr<const Shader> GraphicsEngine::GetShader(const eShaderType aShaderType)
	{
		std::shared_ptr<const Shader> shader = nullptr;

		switch (aShaderType)
		{
		case eShaderType::Unlit_Default:
			shader = GetShader("DefaultPS.cso", "DefaultVS.cso");
			break;
		case eShaderType::Unlit_Animated:
			shader = GetShader("DefaultPS.cso", "AnimatedModelVS.cso");
			break;
		case eShaderType::PBR_Default:
			shader = GetShader("DefaultPBRPS.cso", "DefaultVS.cso");
			break;
		case eShaderType::PBR_Animated:
			shader = GetShader("DefaultPBRPS.cso", "AnimatedModelVS.cso");
			break;
		case eShaderType::SkyBox:
			shader = GetShader("SkyBoxPS.cso", "DefaultVS.cso");
			break;
		case eShaderType::GBuffer:
			shader = GetShader("GBufferPS.cso", "GBufferVS.cso");
			break;
		case eShaderType::Deferred:
			shader = GetShader("DeferredPS.cso", "FullScreenVS.cso");
			break;
		default:
			break;
		}

		return shader;
	}

	Drawer::Renderer* GraphicsEngine::GetRenderer()
	{
		return myRenderer.get();
	}

	ModelFactory* GraphicsEngine::GetModelFactory()
	{
		return myModelFactory.get();
	}

	std::shared_ptr<Graphics::Camera> GraphicsEngine::GetCurrentCamera()
	{
		return myCurrentCamera;
	}

	std::shared_ptr<Graphics::Camera> GraphicsEngine::GetEditorCamera()
	{
		return myEditorCamera;
	}

	std::shared_ptr<Graphics::Camera> GraphicsEngine::GetShadowCamera()
	{
		return myShadowCamera;
	}

	ComPtr<ID3D11Device> GraphicsEngine::GetDevice()
	{
		return myDevice;
	}

	ComPtr<ID3D11DeviceContext> GraphicsEngine::GetContext()
	{
		return myContext;
	}

	ComPtr<ID3D11ShaderResourceView> GraphicsEngine::GetShaderResourceView(const eRenderTargetType aRenderTargetType, const size_t aIndex)
	{
		return myRenderTargets[static_cast<size_t>(aRenderTargetType)][aIndex].shaderResourceView;
	}

	ComPtr<ID3D11DepthStencilView> GraphicsEngine::GetDepthBuffer()
	{
		return myDepthBuffer;
	}

	Math::Vector4f GraphicsEngine::GetAmbientLightColorAndIntensity() const
	{
		return myLightBufferData->ambientLightColorAndIntensity;
	}

	Math::Vector4f GraphicsEngine::GetDirectionalLightColor() const
	{
		return myLightBufferData->directionalLightColorAndIntensity;
	}

	Math::Vector3f GraphicsEngine::GetDirectionalLightDirection() const
	{
		return myLightBufferData->directionalLightDirection;
	}

	unsigned int GraphicsEngine::GetFPSLevelCap() const
	{
		return myFPSLevelCap;
	}

	void GraphicsEngine::CreateViewport(const Math::Vector2ui aSize)
	{
		std::shared_ptr<D3D11_VIEWPORT> viewport = std::make_shared<D3D11_VIEWPORT>();

		viewport->TopLeftX = 0.0f;
		viewport->TopLeftY = 0.0f;
		viewport->Width = static_cast<float> (aSize.x);
		viewport->Height = static_cast<float> (aSize.y);
		viewport->MinDepth = 0.0f;
		viewport->MaxDepth = 1.0f;

		myViewPort = viewport;
	}

	bool GraphicsEngine::IsVSyncActive() const
	{
		return myVSync;
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
		JointsBufferData bonesBufferData;

		if (!myJointsConstantBuffer->Init(sizeof(JointsBufferData), &bonesBufferData))
			assert(false && "Failed to create BoneConstantBuffer");
	}

	void GraphicsEngine::CreateGBuffer(const Math::Vector2ui aResolution)
	{
		std::array<DXGI_FORMAT, 5> formats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT, //Albedo
			DXGI_FORMAT_R10G10B10A2_UNORM, //Normal
			DXGI_FORMAT_R8G8B8A8_UNORM, //Material
			DXGI_FORMAT_R32G32B32A32_FLOAT, //Position
			DXGI_FORMAT_R8G8B8A8_UNORM // AmbientOcclusionAndCustom (R in used, G,B,A unused)
		};

		myRenderTargets[static_cast<size_t>(eRenderTargetType::GBuffer)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::GBuffer)] = CreateRenderTargets(formats.size(), &formats[0], aResolution);
	}

	void GraphicsEngine::CreateDeferredBuffer(const Math::Vector2ui aResolution)
	{
		std::array<DXGI_FORMAT, 1> formats =
		{
			DXGI_FORMAT_R16G16B16A16_FLOAT
		};

		myRenderTargets[static_cast<size_t>(eRenderTargetType::Deferred)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::Deferred)] = CreateRenderTargets(formats.size(), &formats[0], aResolution);
	}

	std::vector<RenderTarget> GraphicsEngine::CreateRenderTargets(const size_t aRenderTargetCount, DXGI_FORMAT* aArrayOfFormats, const Math::Vector2ui& aResolution)
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };

		desc.Width = aResolution.x;
		desc.Height = aResolution.y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		std::vector<RenderTarget> renderTargets(aRenderTargetCount);

		for (size_t i = 0; i < aRenderTargetCount; ++i)
		{
			desc.Format = aArrayOfFormats[i];

			RenderTarget renderTarget;
			ID3D11Texture2D* texture;

			HRESULT result = myDevice->CreateTexture2D(&desc, nullptr, &texture);
			assert(SUCCEEDED(result) && "Failed to create Texture2D");

			result = myDevice->CreateShaderResourceView(texture, nullptr, renderTarget.shaderResourceView.GetAddressOf());
			assert(SUCCEEDED(result) && "Failed to create ShaderResourceView");

			result = myDevice->CreateRenderTargetView(texture, nullptr, renderTarget.renderTargetView.GetAddressOf());
			assert(SUCCEEDED(result) && "Failed to create RenderTargetView");

			texture->Release();

			renderTargets[i] = renderTarget;
		}

		return renderTargets;
	}

	void GraphicsEngine::CreateSwapChain(HWND& aWindowHandle, const Math::Vector2ui aSize)
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = aSize.x;
		swapChainDesc.BufferDesc.Height = aSize.y;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
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
		if (myRenderTargets[static_cast<size_t>(eRenderTargetType::Backbuffer)].empty())
		{
			myRenderTargets[static_cast<size_t>(eRenderTargetType::Backbuffer)].resize(1);
		}

		ID3D11Texture2D* backBufferTexture = nullptr;

		HRESULT result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
		assert(SUCCEEDED(result) && "Failed to get Backbuffer");

		result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, myRenderTargets[static_cast<size_t>(eRenderTargetType::Backbuffer)][0].renderTargetView.GetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create Backbuffer");

		backBufferTexture->Release();
	}

	void GraphicsEngine::CreateDepthBuffer(const Math::Vector2ui aSize)
	{
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = aSize.x;
		descDepth.Height = aSize.y;
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
		CameraBufferData cameraBuffer;

		cameraBuffer.worldToClipMatrix = Math::Matrix4x4f::Identity();
		cameraBuffer.cameraPosition = Math::Vector3f{ 0.0f,0.0f,0.0f };

		if (!myCameraConstantBuffer->Init(sizeof(CameraBufferData), &cameraBuffer))
			assert(false && "Failed to create CameraConstantBuffer");
	}

	void GraphicsEngine::CreateTimeBuffer()
	{
		TimeBufferData timeBuffer;

		timeBuffer.totalTime = 0.0f;
		timeBuffer.deltaTime = 0.0f;

		if (!myTimeConstantBuffer->Init(sizeof(TimeBufferData), &timeBuffer))
			assert(false && "Failed to create TimeConstantBuffer");
	}

	void GraphicsEngine::CreateLightBuffer()
	{
		LightBufferData lightBufferData;

		lightBufferData.directionalLightWorldToProjectionMatrix = Math::Matrix4x4f::Identity();
		lightBufferData.directionalLightColorAndIntensity = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		lightBufferData.directionalLightDirection = Math::Vector3f(0.0f, 0.0f, 0.0f);

		if (myLightConstantBuffer->Init(sizeof(LightBufferData), &lightBufferData) == false)
		{
			assert(false && "Failed to create LightConstantBuffer");
		}
	}
}