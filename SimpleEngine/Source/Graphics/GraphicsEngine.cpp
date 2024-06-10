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
		, myCurrentRasterizerState(eRasterizerState::BackfaceCulling)
	{
	}

	GraphicsEngine::~GraphicsEngine()
	{
	}

	const bool GraphicsEngine::Init(HWND& aWindowHandle, const Math::Vector2ui& aWindowSize)
	{
		myCameraConstantBuffer = std::make_unique<ConstantBuffer>();
		myTimeConstantBuffer = std::make_unique<ConstantBuffer>();
		myJointsConstantBuffer = std::make_unique<ConstantBuffer>();
		myLightConstantBuffer = std::make_unique<ConstantBuffer>();
		myPostProcessConstantBuffer = std::make_unique<ConstantBuffer>();

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
		CreateGRenderTarget(aWindowSize);
		CreateDeferredRenderTarget(aWindowSize);
		CreatePostProcessingRenderTarget(aWindowSize);
		CreateBloomDownAndUpSampleRenderTarget(aWindowSize);
		CreateBloomRenderTarget(aWindowSize);

		CreateDepthBuffer(aWindowSize);
		CreateDepthStencilState();
		CreateRasterizerStates();
		CreateSamplerState();
		CreateBlendStates();

		CreateCameraConstantBuffer();
		CreateTimeConstantBuffer();
		CreateLightConstantBuffer();
		CreateJointsConstantBuffer();
		CreatePostProcessingConstantBuffer();

		PreloadTextures();
		PreloadShaders();

		myEditorCamera->Init();
		myImGuiEngine->Init();
		myRenderer->Init();
		myModelFactory->Init();

		SetRasterizerState(eRasterizerState::BackfaceCulling);
		SetDepthStencilState(eDepthStencilState::Less_Equal);
		SetSamplerState(eSamplerState::Bilinear_Warp);

		myContext->RSSetViewports(1, myViewPort.get());

		myCameraConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Camera);
		myTimeConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Time);
		myLightConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Light);
		myJointsConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Joints);
		myPostProcessConstantBuffer->SetSlot(5);

		myLightBufferData->directionalLightDirection.x = 0.0f;
		myLightBufferData->directionalLightDirection.y = -1.0f;
		myLightBufferData->directionalLightDirection.z = 0.0f;

		myCurrentCamera = myEditorCamera;

		return true;
	}

	void GraphicsEngine::PrepareFrame()
	{
		ClearDepthStencilView();
		ClearRenderTarget(eRenderTargetType::GBuffer);
		ClearRenderTarget(eRenderTargetType::PostProcessing);
		ClearPointLightCount();

		UpdateTimeConstantBuffer();
		UpdateCameraConstantBuffer();

		{
			myPostProcessConstantBuffer->Bind(myPostProcessConstantBuffer->GetSlot());
			myPostProcessConstantBuffer->Update(sizeof(PostProcessData), &myPostProcessData);
		}
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
		if (!AddTexture("Assets\\Textures\\T_SimpleTexture_C.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\T_Cat_C.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\T_Hamster_C.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\T_CatScared_C.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_CloudAnime_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_NightStars_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_DayCloud_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_AutumnForest_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_Skansen_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");
	}

	void GraphicsEngine::PreloadShaders()
	{
		if (!AddShader("DefaultPS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("DefaultPS.cso", "AnimatedModelVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("LinePS.cso", "Line2DVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("SkyBoxPS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("GBufferPS.cso", "GBufferVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("DeferredPS.cso", "FullScreenVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("PointLightCullPS.cso", "DefaultVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("PostProcessingPS.cso", "FullScreenVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("FullScreenCopyPS.cso", "FullScreenVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("GaussianBlurPS.cso", "FullScreenVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("BloomPixelFilterPS.cso", "FullScreenVS.cso"))
			assert(false && "Failed to add Shader");

		if (!AddShader("BloomPS.cso", "FullScreenVS.cso"))
			assert(false && "Failed to add Shader");
	}

	void GraphicsEngine::FilterPixelForBloom()
	{
		constexpr size_t shaderResourceViewCount = 1;

		SetRenderTarget(eRenderTargetType::Bloom);

		const std::shared_ptr<const Shader> bloomPixelFilterShader = GetShader(eShaderType::BloomPixelFilter);
		bloomPixelFilterShader->BindThisShader(myContext.Get());

		ID3D11ShaderResourceView* shaderResourceViews[shaderResourceViewCount] = {};
		shaderResourceViews[0] = GetRenderTargets(eRenderTargetType::Deferred)[0].shaderResourceView.Get();
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, shaderResourceViewCount, shaderResourceViews);

		RenderFullScreenQuad();

		ID3D11ShaderResourceView* nullViews = nullptr;
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, shaderResourceViewCount, &nullViews);
	}

	void GraphicsEngine::DownAndUpSampleForBloom()
	{
		const Math::Vector2ui currentResolution = Global::GetResolution();
		Math::Vector2ui downScaledResolution = currentResolution;

		D3D11_VIEWPORT viewport = {};
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		ID3D11RenderTargetView* renderTargetPointer = nullptr;

		SetBlendState(eBlendState::Disabled);

		for (size_t i = 0; i < myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)].size(); ++i)
		{
			downScaledResolution = downScaledResolution / 2;

			viewport.Width = static_cast<float>(downScaledResolution.x);
			viewport.Height = static_cast<float>(downScaledResolution.y);

			myContext->RSSetViewports(1, &viewport);

			UnbindAllRenderTargets();

			renderTargetPointer = myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)][i].renderTargetView.Get();
			myContext->OMSetRenderTargets(1, &renderTargetPointer, nullptr);

			ID3D11ShaderResourceView* shaderResources[1] = {};

			if (i == 0)
			{
				shaderResources[0] = myRenderTargets[static_cast<size_t>(eRenderTargetType::Bloom)][0].shaderResourceView.Get();
			}
			else
			{
				shaderResources[0] = myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)][i - 1].shaderResourceView.Get();
			}

			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, shaderResources);

			const std::shared_ptr<const Graphics::Shader> gaussianBlurShader = GetShader(Graphics::eShaderType::GaussianBlur);
			gaussianBlurShader->BindThisShader(myContext.Get());

			RenderFullScreenQuad();

			ID3D11ShaderResourceView* nullViews = nullptr;
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
		}

		SetBlendState(eBlendState::AlphaBlend);

		for (size_t i = myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)].size(); i > 1; --i)
		{
			downScaledResolution.x = downScaledResolution.x * 2;
			downScaledResolution.y = downScaledResolution.y * 2;

			viewport.Width = static_cast<float>(downScaledResolution.x);
			viewport.Height = static_cast<float>(downScaledResolution.y);

			myContext->RSSetViewports(1, &viewport);

			UnbindAllRenderTargets();

			renderTargetPointer = myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)][i - 2].renderTargetView.Get();
			myContext->OMSetRenderTargets(1, &renderTargetPointer, nullptr);

			ID3D11ShaderResourceView* shaderResources[1] = {};
			shaderResources[0] = myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)][i - 1].shaderResourceView.Get();
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, shaderResources);

			const std::shared_ptr<const Graphics::Shader> shader = GetShader(Graphics::eShaderType::GaussianBlur);
			shader->BindThisShader(myContext.Get());

			RenderFullScreenQuad();

			ID3D11ShaderResourceView* nullViews = nullptr;
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
		}

		SetBlendState(eBlendState::Disabled);

		myContext->RSSetViewports(1, myViewPort.get());
	}

	void GraphicsEngine::RenderBloom()
	{
		constexpr size_t shaderResourceViewCount = 2;

		SetRenderTarget(eRenderTargetType::Bloom);

		for (size_t i = 0; i < myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)].size(); ++i)
		{
			ID3D11ShaderResourceView* shaderResourceViews[shaderResourceViewCount] = {};
			shaderResourceViews[0] = myRenderTargets[static_cast<size_t>(eRenderTargetType::Deferred)][0].shaderResourceView.Get();
			shaderResourceViews[1] = myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)][i].shaderResourceView.Get();
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, shaderResourceViewCount, shaderResourceViews);

			const std::shared_ptr<const Graphics::Shader> shader = GetShader(Graphics::eShaderType::Bloom);
			shader->BindThisShader(myContext.Get());

			RenderFullScreenQuad();

			ID3D11ShaderResourceView* nullViews[2] = { nullptr };
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 2, nullViews);
		}
	}

	void GraphicsEngine::ClearPointLightCount()
	{
		myLightBufferData->currentPointLightCount = 0;
	}

	void GraphicsEngine::ClearRenderTarget(const eRenderTargetType aRenderTargetType)
	{
		const std::vector<RenderTarget>& renderTargets = myRenderTargets[static_cast<size_t>(aRenderTargetType)];

		for (size_t i = 0; i < renderTargets.size(); ++i)
		{
			myContext->ClearRenderTargetView(renderTargets[i].renderTargetView.Get(), &myClearColor[0]);
		}
	}

	void GraphicsEngine::ClearDepthStencilView()
	{
		myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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

		if (SimpleUtilities::FindSuffix(aFileName, "_C"))
		{
			texture->SetSlot(Global_Slot_Albedo);
		}
		else if (SimpleUtilities::FindSuffix(aFileName, "_N"))
		{
			texture->SetSlot(Global_Slot_Normal);
		}
		else if (SimpleUtilities::FindSuffix(aFileName, "_M"))
		{
			texture->SetSlot(Global_Slot_Material);
		}
		else
		{
			texture->SetSlot(aSlot);
		}

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

	void GraphicsEngine::ApplyAmbientAndDirectionalLightDeferred(const eRenderTargetType aRenderTargetType)
	{
		static constexpr size_t gBufferCount = Global_GBuffer_Count;

		std::vector<Graphics::RenderTarget>& gBuffers = myRenderTargets[static_cast<size_t>(aRenderTargetType)];

		ID3D11ShaderResourceView* shaderResources[gBufferCount] = {};

		for (size_t i = 0; i < gBufferCount; ++i)
		{
			shaderResources[i] = gBuffers[i].shaderResourceView.Get();
		}

		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, gBufferCount, shaderResources);

		std::shared_ptr<const Shader> shader = GetShader(eShaderType::Deferred);
		shader->BindThisShader(myContext.Get());

		RenderFullScreenQuad();

		ID3D11ShaderResourceView* nullSRVs[gBufferCount] = { NULL };
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, gBufferCount, nullSRVs);
	}

	void GraphicsEngine::RenderFullScreenQuad()
	{
		myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myContext->IASetInputLayout(nullptr);
		myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		myContext->GSSetShader(nullptr, nullptr, 0);
		myContext->Draw(3, 0);
	}

	void GraphicsEngine::ApplyPostProcessing(const eRenderTargetType aRenderTargetType)
	{
		ID3D11ShaderResourceView* shaderResources[1] = {};
		shaderResources[0] = GetRenderTargets(aRenderTargetType)[0].shaderResourceView.Get();
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, shaderResources);

		const std::shared_ptr<const Graphics::Shader> shader = GetShader(Graphics::eShaderType::PostProcessing);
		shader->BindThisShader(myContext.Get());

		RenderFullScreenQuad();

		ID3D11ShaderResourceView* nullViews = nullptr;
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
	}

	void GraphicsEngine::ApplyBloom()
	{
		if (myPostProcessData.useBloom == false)
		{
			SetRenderTarget(eRenderTargetType::Bloom);
			RenderFullScreenCopy(eRenderTargetType::Deferred);
			return;
		}

		FilterPixelForBloom();

		SetSamplerState(eSamplerState::Trilinear_Clamp);

		DownAndUpSampleForBloom();
		RenderBloom();

		SetBlendState(eBlendState::Disabled);
		SetSamplerState(eSamplerState::Bilinear_Warp);
	}

	void GraphicsEngine::RenderFullScreenCopy(const eRenderTargetType aRenderTargetType)
	{
		ID3D11ShaderResourceView* shaderResources[1] = {};
		shaderResources[0] = GetRenderTargets(aRenderTargetType)[0].shaderResourceView.Get();
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, shaderResources);

		const std::shared_ptr<const Graphics::Shader> shader = GetShader(Graphics::eShaderType::Copy);
		shader->BindThisShader(myContext.Get());

		RenderFullScreenQuad();

		ID3D11ShaderResourceView* nullViews = nullptr;
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
	}

	void GraphicsEngine::AddPointLight(const PointLightData& aPointLightData)
	{
		myLightBufferData->pointLightData[myLightBufferData->currentPointLightCount] = aPointLightData;
		++myLightBufferData->currentPointLightCount;
	}

	PointLightData* GraphicsEngine::GetPointLightDataArray() const
	{
		return myLightBufferData->pointLightData;
	}

	size_t GraphicsEngine::GetPointLightCount() const
	{
		return myLightBufferData->currentPointLightCount;
	}

	void GraphicsEngine::UnbindAllRenderTargets()
	{
		static constexpr size_t maxRenderTargetSupportedByDX11 = 8;
		ID3D11RenderTargetView* nullViews[maxRenderTargetSupportedByDX11] = { nullptr };
		myContext->OMSetRenderTargets(maxRenderTargetSupportedByDX11, nullViews, nullptr);
	}

	void GraphicsEngine::UpdateCameraConstantBuffer()
	{
		CameraBufferData frameBuffer = {};
		frameBuffer.worldToClipMatrix = myCurrentCamera->GetWorldToClipMatrix();
		frameBuffer.cameraPosition = myCurrentCamera->GetPosition();
		frameBuffer.resolution = Global::GetResolution();

		myCameraConstantBuffer->Bind(myCameraConstantBuffer->GetSlot());
		myCameraConstantBuffer->Update(sizeof(CameraBufferData), &frameBuffer);
	}

	void GraphicsEngine::UpdateTimeConstantBuffer()
	{
		TimeBufferData timeBuffer = {};
		timeBuffer.totalTime = static_cast<float>(Global::GetTotalTime());
		timeBuffer.deltaTime = static_cast<float>(Global::GetDeltaTime());
		myTimeConstantBuffer->Bind(myTimeConstantBuffer->GetSlot());
		myTimeConstantBuffer->Update(sizeof(TimeBufferData), &timeBuffer);
	}

	void GraphicsEngine::UpdateLightBuffer(const size_t aLightIndex)
	{
		LightBufferData lightBufferData;

		lightBufferData.directionalLightWorldToProjectionMatrix = Math::Matrix4x4f::GetFastInverse(myShadowCamera->GetMatrix()) * myShadowCamera->GetProjectionMatrix();
		lightBufferData.ambientLightColorAndIntensity = myLightBufferData->ambientLightColorAndIntensity;
		lightBufferData.directionalLightColorAndIntensity = myLightBufferData->directionalLightColorAndIntensity;
		lightBufferData.directionalLightDirection = myLightBufferData->directionalLightDirection;
		lightBufferData.currentPointLightCount = myLightBufferData->currentPointLightCount;

		for (size_t i = 0; i < myLightBufferData->currentPointLightCount; i++)
		{
			lightBufferData.pointLightData[i] = myLightBufferData->pointLightData[aLightIndex];
		}

		myLightConstantBuffer->Bind(myLightConstantBuffer->GetSlot());
		myLightConstantBuffer->Update(sizeof(LightBufferData), &lightBufferData);
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

		{ //TO-DO(v10.0.5): Figure a out to resize buffers properly
			CreateBackBuffer();
			CreateViewport(newWindowSize);
			CreateDepthBuffer(newWindowSize);
			CreateGRenderTarget(newWindowSize);
			CreateDeferredRenderTarget(newWindowSize);
			CreatePostProcessingRenderTarget(newWindowSize);
			CreateBloomDownAndUpSampleRenderTarget(newWindowSize);
			CreateBloomRenderTarget(newWindowSize);
		}

		myContext->RSSetViewports(1, myViewPort.get());
	}

	void GraphicsEngine::SetRenderTarget(eRenderTargetType aRenderTargetType, ID3D11DepthStencilView* aDepthBuffer)
	{
		UnbindAllRenderTargets();

		const std::vector<RenderTarget>& originalRenderTargets = myRenderTargets[static_cast<size_t>(aRenderTargetType)];
		const size_t count = originalRenderTargets.size();

		std::vector<ID3D11RenderTargetView*> renderTargetsPointer(count);

		for (size_t i = 0; i < count; ++i)
		{
			renderTargetsPointer[i] = originalRenderTargets[i].renderTargetView.Get();
		}

		myContext->OMSetRenderTargets(static_cast<unsigned int>(count), &renderTargetsPointer[0], aDepthBuffer);
	}

	void GraphicsEngine::SetCamera(std::shared_ptr<Graphics::Camera> aCamera)
	{
		myCurrentCamera = aCamera;
		UpdateCameraConstantBuffer();
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
		myCurrentRasterizerState = aRasterizerState;
		myContext->RSSetState(myRasterizerStates[static_cast<int>(myCurrentRasterizerState)].Get());
	}

	void GraphicsEngine::SetBlendState(const eBlendState aBlendState)
	{
		myContext->OMSetBlendState(myBlendStates[static_cast<size_t>(aBlendState)].Get(), nullptr, 0xffffffff);
	}

	void GraphicsEngine::SetDepthStencilState(const eDepthStencilState aDepthStencilState)
	{
		myContext->OMSetDepthStencilState(myDepthStencilStates[static_cast<size_t>(aDepthStencilState)].Get(), 0);
	}

	void GraphicsEngine::SetSamplerState(const eSamplerState aSamplerState)
	{
		myContext->PSSetSamplers(0, 1, mySamplerStates[static_cast<size_t>(aSamplerState)].GetAddressOf());
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

	void GraphicsEngine::SetUseToneMapping(const bool aShouldUseToneMapping)
	{
		myPostProcessData.useToneMapping = aShouldUseToneMapping;
	}

	void GraphicsEngine::SetUseBloom(const bool aShouldUseBloom)
	{
		myPostProcessData.useBloom = aShouldUseBloom;
	}

	void GraphicsEngine::SetBloomPixelThreshold(const float aValue)
	{
		myPostProcessData.bloomPixelFilterThreshold = aValue;
	}

	void GraphicsEngine::SetSaturation(const float aValue)
	{
		myPostProcessData.saturation = aValue;
	}

	void GraphicsEngine::SetExposure(const float aValue)
	{
		myPostProcessData.exposure = aValue;
	}

	void GraphicsEngine::SetContrast(const float aValue)
	{
		myPostProcessData.contrast = aValue;
	}

	void GraphicsEngine::SetBlackPoint(const float aValue)
	{
		myPostProcessData.blackpoint = aValue;
	}

	void GraphicsEngine::SetBloom(const float aValue)
	{
		myPostProcessData.bloom = aValue;
	}

	void GraphicsEngine::SetTint(const Math::Vector3f& aColor)
	{
		myPostProcessData.tint = aColor;
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
			unsigned int slot = Graphics::Global_Slot_Albedo;

			if (SimpleUtilities::FindSuffix(aFilePath, "_C"))
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
			else if (SimpleUtilities::FindSuffix(aFilePath, "_E"))
			{
				slot = Global_Slot_CubeMap;
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
			texture = GetTexture("Assets\\Textures\\T_SimpleTexture_C.dds");
			break;
		case eTextureType::Simple:
			texture = GetTexture("Assets\\Textures\\T_SimpleTexture_C.dds");
			break;
		case eTextureType::SkyBox_DayCloud:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_DayCloud_E.dds");
			break;
		case eTextureType::SkyBox_NightStar:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_NightStars_E.dds");
			break;
		case eTextureType::SkyBox_DayGrassland:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_CloudAnime_E.dds");
			break;
		case eTextureType::SkyBox_AutumnForest:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_AutumnForest_E.dds");
			break;
		case eTextureType::TGA_Skansen:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_Skansen_E.dds");
			break;
		}

		return texture;
	}

	std::shared_ptr<const Texture> GraphicsEngine::GetIcon(const eIconType aTextureType)
	{
		std::shared_ptr<const Texture> texture = nullptr;

		switch (aTextureType)
		{
		case eIconType::FBX:
			texture = GetTexture("Assets\\Textures\\Editor\\Icon_FBX.dds");
			break;
		case eIconType::Folder:
			texture = GetTexture("Assets\\Textures\\Editor\\Icon_Folder.dds");
			break;
		case eIconType::CubeMap:
			texture = GetTexture("Assets\\Textures\\Editor\\Icon_CubeMap.dds");
			break;
		case eIconType::PNG:
			texture = GetTexture("Assets\\Textures\\Editor\\Icon_PNG.dds");
			break;
		case eIconType::JPG:
			texture = GetTexture("Assets\\Textures\\Editor\\Icon_JPG.dds");
			break;
		case eIconType::OBJ:
			texture = GetTexture("Assets\\Textures\\Editor\\Icon_OBJ.dds");
			break;
		case eIconType::MP3:
			texture = GetTexture("Assets\\Textures\\Editor\\Icon_MP3.dds");
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
		case eShaderType::PointLight:
			shader = GetShader("PointLightCullPS.cso", "DefaultVS.cso");
			break;
		case eShaderType::PostProcessing:
			shader = GetShader("PostProcessingPS.cso", "FullScreenVS.cso");
			break;
		case eShaderType::GaussianBlur:
			shader = GetShader("GaussianBlurPS.cso", "FullScreenVS.cso");
			break;
		case eShaderType::Bloom:
			shader = GetShader("BloomPS.cso", "FullScreenVS.cso");
			break;
		case eShaderType::BloomPixelFilter:
			shader = GetShader("BloomPixelFilterPS.cso", "FullScreenVS.cso");
			break;
		case eShaderType::Copy:
			shader = GetShader("FullScreenCopyPS.cso", "FullScreenVS.cso");
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

	const eRasterizerState GraphicsEngine::GetCurrentRasterizerState() const
	{
		return myCurrentRasterizerState;
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

	const PostProcessData& GraphicsEngine::GetPostProcessData() const
	{
		return myPostProcessData;
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
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::Wireframe)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: Wireframe");

		rasterizerDesc = {};
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::WireframeNoCulling)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: WireframeNoCulling");

		rasterizerDesc = {};
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::NoFaceCulling)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: NoFaceCulling");

		rasterizerDesc = {};
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::FrontFaceCulling)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: FrontFaceCulling");

		myRasterizerStates[static_cast<int>(eRasterizerState::BackfaceCulling)] = nullptr;
	}

	void GraphicsEngine::CreateJointsConstantBuffer()
	{
		JointsBufferData bonesBufferData;

		if (myJointsConstantBuffer->Init(sizeof(JointsBufferData), &bonesBufferData) == false)
			assert(false && "Failed to create BoneConstantBuffer");
	}

	void GraphicsEngine::CreateGRenderTarget(const Math::Vector2ui aResolution)
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

	void GraphicsEngine::CreateDeferredRenderTarget(const Math::Vector2ui aResolution)
	{
		std::array<DXGI_FORMAT, 1> formats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT
		};

		myRenderTargets[static_cast<size_t>(eRenderTargetType::Deferred)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::Deferred)] = CreateRenderTargets(formats.size(), &formats[0], aResolution);
	}

	void GraphicsEngine::CreatePostProcessingRenderTarget(const Math::Vector2ui aResolution)
	{
		std::array<DXGI_FORMAT, 1> formats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT
		};

		myRenderTargets[static_cast<size_t>(eRenderTargetType::PostProcessing)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::PostProcessing)] = CreateRenderTargets(formats.size(), &formats[0], aResolution);
	}

	void GraphicsEngine::CreateBloomDownAndUpSampleRenderTarget(const Math::Vector2ui aResolution)
	{
		std::array<DXGI_FORMAT, 5> formats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT
		};

		D3D11_TEXTURE2D_DESC desc = { 0 };

		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		Math::Vector2ui resolution = aResolution;

		std::vector<RenderTarget> renderTargets(formats.size());

		for (size_t i = 0; i < formats.size(); ++i)
		{
			resolution.x = resolution.x / 2;
			resolution.y = resolution.y / 2;

			desc.Width = resolution.x;
			desc.Height = resolution.y;

			desc.Format = formats[i];

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

		myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)] = renderTargets;
	}

	void GraphicsEngine::CreateBloomRenderTarget(const Math::Vector2ui aResolution)
	{
		std::array<DXGI_FORMAT, 1> formats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT
		};

		myRenderTargets[static_cast<size_t>(eRenderTargetType::Bloom)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::Bloom)] = CreateRenderTargets(formats.size(), &formats[0], aResolution);
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

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		result = myDevice->CreateRenderTargetView(backBufferTexture, &rtvDesc, myRenderTargets[static_cast<size_t>(eRenderTargetType::Backbuffer)][0].renderTargetView.GetAddressOf());
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
		depthStencilDesc.StencilEnable = false;

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT result = myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStencilStates[static_cast<size_t>(eDepthStencilState::Less_Equal)].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create DepthStencilState");

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
		result = myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStencilStates[static_cast<size_t>(eDepthStencilState::Greater)].GetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create DepthStencilState");
	}

	void GraphicsEngine::CreateSamplerState()
	{
		HRESULT result = S_OK;

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		result = myDevice->CreateSamplerState(&samplerDesc, &mySamplerStates[static_cast<size_t>(eSamplerState::Bilinear_Warp)]);
		assert(SUCCEEDED(result) && "Failed to create SamplerState");

		D3D11_SAMPLER_DESC samplerDesc2 = {};
		samplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc2.MipLODBias = 0.0f;
		samplerDesc2.MaxAnisotropy = 16;
		samplerDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc2.BorderColor[0] = 0;
		samplerDesc2.BorderColor[1] = 0;
		samplerDesc2.BorderColor[2] = 0;
		samplerDesc2.BorderColor[3] = 0;
		samplerDesc2.MinLOD = 0;
		samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;

		result = myDevice->CreateSamplerState(&samplerDesc2, &mySamplerStates[static_cast<size_t>(eSamplerState::Trilinear_Clamp)]);
		assert(SUCCEEDED(result) && "Failed to create SamplerState");
	}

	void GraphicsEngine::CreateCameraConstantBuffer()
	{
		CameraBufferData cameraBuffer;

		cameraBuffer.worldToClipMatrix = Math::Matrix4x4f::Identity();
		cameraBuffer.cameraPosition = Math::Vector3f{ 0.0f,0.0f,0.0f };

		if (!myCameraConstantBuffer->Init(sizeof(CameraBufferData), &cameraBuffer))
			assert(false && "Failed to create CameraConstantBuffer");
	}

	void GraphicsEngine::CreateTimeConstantBuffer()
	{
		TimeBufferData timeBuffer;

		timeBuffer.totalTime = 0.0f;
		timeBuffer.deltaTime = 0.0f;

		if (!myTimeConstantBuffer->Init(sizeof(TimeBufferData), &timeBuffer))
			assert(false && "Failed to create TimeConstantBuffer");
	}

	void GraphicsEngine::CreateLightConstantBuffer()
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

	void GraphicsEngine::CreatePostProcessingConstantBuffer()
	{
		PostProcessData postProcessingData;

		if (myPostProcessConstantBuffer->Init(sizeof(LightBufferData), &postProcessingData) == false)
		{
			assert(false && "Failed to create LightConstantBuffer");
		}
	}

	void GraphicsEngine::CreateBlendStates()
	{
		HRESULT result = S_OK;

		D3D11_BLEND_DESC blendStateDescription = {};
		blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
		blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = myDevice->CreateBlendState(&blendStateDescription, myBlendStates[static_cast<size_t>(eBlendState::Disabled)].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create blend state");

		D3D11_BLEND_DESC blendStateDescription2 = {};
		blendStateDescription2.RenderTarget[0].BlendEnable = TRUE;
		blendStateDescription2.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription2.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendStateDescription2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription2.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription2.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription2.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		blendStateDescription2.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = myDevice->CreateBlendState(&blendStateDescription2, myBlendStates[static_cast<size_t>(eBlendState::AdditiveBlend)].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create blend state");

		D3D11_BLEND_DESC blendStateDescription3 = {};
		blendStateDescription3.RenderTarget[0].BlendEnable = TRUE;
		blendStateDescription3.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription3.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDescription3.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription3.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription3.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription3.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		blendStateDescription3.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = myDevice->CreateBlendState(&blendStateDescription, myBlendStates[static_cast<size_t>(eBlendState::AlphaBlend)].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create blend state");
	}
}