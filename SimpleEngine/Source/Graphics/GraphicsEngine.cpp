#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/GraphicsEngine.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Graphics/Managers/ConstantBufferManager.hpp"
#include "Engine/Global.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include <External/imgui.h>
#include <External/nlohmann/json.hpp>
#include <fstream>

#ifdef _DEBUG
#define REPORT_DX_WARNINGS
#endif

namespace Graphics
{
	GraphicsEngine::GraphicsEngine()
		: myCurrentCameraRaw(nullptr)
	{
	}

	GraphicsEngine::~GraphicsEngine()
	{
	}

	void GraphicsEngine::Init(HWND& aWindowHandle, const Math::Vector2ui& aWindowSize)
	{
		myGenericDataManager = std::make_unique<GenericDataManager>();
		myRenderTargetManager = std::make_unique<RenderTargetManager>();
		myStateManager = std::make_unique<StateManager>();
		myShaderManager = std::make_unique<ShaderManager>();
		myConstantBufferManager = std::make_unique<ConstantBufferManager>();
		myTextureManager = std::make_unique<TextureManager>();
		myLightManager = std::make_unique<LightManager>();
		myImGuiEngine = std::make_unique<Simple::ImGuiEngine>();
		myEditorCamera = std::make_shared<Graphics::Camera>();
		myDefaultCamera = std::make_shared<Graphics::Camera>();
		myRenderer = std::make_unique<Drawer::Renderer>();
		myModelFactory = std::make_unique<ModelFactory>();

		LoadSettingsFromJson();

		CreateSwapChain(aWindowHandle, aWindowSize);

		myGenericDataManager->Init();
		myRenderTargetManager->Init(myDevice, mySwapChain, myContext, aWindowSize);
		myStateManager->Init(myDevice);
		myShaderManager->Init(myDevice);
		myConstantBufferManager->Init();
		myTextureManager->Init();
		myLightManager->Init();
		myEditorCamera->Init();
		myDefaultCamera->Init();
		myImGuiEngine->Init();
		myRenderer->Init();
		myModelFactory->Init();

		PreloadShaders();

		myStateManager->SetRasterizerState(myContext, eRasterizerState::BackfaceCulling);
		myStateManager->SetDepthStencilState(myContext, eDepthStencilState::Less_Equal);
		myStateManager->SetSamplerState(myContext, eSamplerState::Bilinear_Warp);

		myCurrentCameraRaw = myEditorCamera.get();
	}

	void GraphicsEngine::PrepareFrame()
	{
		myRenderTargetManager->ClearDepthStencilView(myContext);
		myRenderTargetManager->ClearAllRenderTargets(myContext, myGenericDataManager->GetClearColor());
		myLightManager->ClearPointLightCount();

		myConstantBufferManager->UpdateTimeConstantBuffer(static_cast<float>(Global::GetTotalTime()), Global::GetDeltaTime());
		myConstantBufferManager->UpdateCameraConstantBuffer(myCurrentCameraRaw, Global::GetResolution());
		myConstantBufferManager->UpdatePostProcessConstantBuffer(myLightManager->GetPostProcessData());
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
		[[maybe_unused]] const HRESULT result = mySwapChain->Present(myGenericDataManager->GetFPSLevelCap(), 0);
		assert(SUCCEEDED(result) && "Failed to present frame");
		PROFILER_END();
	}

	void GraphicsEngine::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		myGenericDataManager->SetVSync(json["Game_Settings"]["VSync"]);
	}

	void GraphicsEngine::PreloadShaders()
	{
		for (size_t i = 0; i < static_cast<size_t>(eShaderType::Count); ++i)
		{
			myShaderManager->GetShader(static_cast<eShaderType>(i));
		}
	}

	void GraphicsEngine::FilterPixelForBloom()
	{
		constexpr size_t shaderResourceViewCount = 1;

		SetRenderTarget(eRenderTargetType::Bloom);

		const std::shared_ptr<const Shader> bloomPixelFilterShader = myShaderManager->GetShader(eShaderType::BloomPixelFilter);
		bloomPixelFilterShader->BindThisShader(myContext.Get());

		ID3D11ShaderResourceView* shaderResourceViews[shaderResourceViewCount] = {};
		shaderResourceViews[0] = myRenderTargetManager->GetRenderTargets(eRenderTargetType::Deferred)[0].shaderResourceView.Get();
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

		myStateManager->SetBlendState(myContext, eBlendState::Disabled);

		const std::vector<RenderTarget>& upDownScaleRenderTargets = myRenderTargetManager->GetRenderTargets(eRenderTargetType::BloomDownAndUpScale);
		const std::vector<RenderTarget>& bloomRenderTarget = myRenderTargetManager->GetRenderTargets(eRenderTargetType::Bloom);

		for (size_t i = 0; i < upDownScaleRenderTargets.size(); ++i)
		{
			downScaledResolution = downScaledResolution / 2;

			viewport.Width = static_cast<float>(downScaledResolution.x);
			viewport.Height = static_cast<float>(downScaledResolution.y);

			myContext->RSSetViewports(1, &viewport);

			myRenderTargetManager->UnbindAllRenderTargets(myContext);

			renderTargetPointer = upDownScaleRenderTargets[i].renderTargetView.Get();
			myContext->OMSetRenderTargets(1, &renderTargetPointer, nullptr);

			ID3D11ShaderResourceView* shaderResources[1] = {};

			if (i == 0)
			{
				shaderResources[0] = bloomRenderTarget[0].shaderResourceView.Get();
			}
			else
			{
				shaderResources[0] = upDownScaleRenderTargets[i - 1].shaderResourceView.Get();
			}

			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, shaderResources);

			const std::shared_ptr<const Graphics::Shader> gaussianBlurShader = myShaderManager->GetShader(Graphics::eShaderType::GaussianBlur);
			gaussianBlurShader->BindThisShader(myContext.Get());

			RenderFullScreenQuad();

			ID3D11ShaderResourceView* nullViews = nullptr;
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
		}

		myStateManager->SetBlendState(myContext, eBlendState::AlphaBlend);

		for (size_t i = upDownScaleRenderTargets.size(); i > 1; --i)
		{
			downScaledResolution.x = downScaledResolution.x * 2;
			downScaledResolution.y = downScaledResolution.y * 2;

			viewport.Width = static_cast<float>(downScaledResolution.x);
			viewport.Height = static_cast<float>(downScaledResolution.y);

			myContext->RSSetViewports(1, &viewport);

			myRenderTargetManager->UnbindAllRenderTargets(myContext);

			renderTargetPointer = upDownScaleRenderTargets[i - 2].renderTargetView.Get();
			myContext->OMSetRenderTargets(1, &renderTargetPointer, nullptr);

			ID3D11ShaderResourceView* shaderResources[1] = {};
			shaderResources[0] = upDownScaleRenderTargets[i - 1].shaderResourceView.Get();
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, shaderResources);

			const std::shared_ptr<const Graphics::Shader> shader = myShaderManager->GetShader(Graphics::eShaderType::GaussianBlur);
			shader->BindThisShader(myContext.Get());

			RenderFullScreenQuad();

			ID3D11ShaderResourceView* nullViews = nullptr;
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
		}

		myStateManager->SetBlendState(myContext, eBlendState::Disabled);

		myContext->RSSetViewports(1, myRenderTargetManager->GetViewPort().get());
	}

	void GraphicsEngine::RenderBloom()
	{
		constexpr size_t shaderResourceViewCount = 2;

		SetRenderTarget(eRenderTargetType::Bloom);

		for (size_t i = 0; i < myRenderTargetManager->GetRenderTargets(eRenderTargetType::BloomDownAndUpScale).size(); ++i)
		{
			ID3D11ShaderResourceView* shaderResourceViews[shaderResourceViewCount] = {};
			shaderResourceViews[0] = myRenderTargetManager->GetRenderTargets(eRenderTargetType::Deferred)[0].shaderResourceView.Get();
			shaderResourceViews[1] = myRenderTargetManager->GetRenderTargets(eRenderTargetType::BloomDownAndUpScale)[i].shaderResourceView.Get();
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, shaderResourceViewCount, shaderResourceViews);

			const std::shared_ptr<const Graphics::Shader> shader = myShaderManager->GetShader(Graphics::eShaderType::Bloom);
			shader->BindThisShader(myContext.Get());

			RenderFullScreenQuad();

			ID3D11ShaderResourceView* nullViews[2] = { nullptr };
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 2, nullViews);
		}
	}

	void GraphicsEngine::ApplyAmbientAndDirectionalLightDeferred(const eRenderTargetType aRenderTargetType)
	{
		static constexpr size_t gBufferCount = Global_GBuffer_Count;

		std::vector<Graphics::RenderTarget>& gBuffers = myRenderTargetManager->GetRenderTargets(aRenderTargetType);

		ID3D11ShaderResourceView* shaderResources[gBufferCount] = {};

		for (size_t i = 0; i < gBufferCount; ++i)
		{
			shaderResources[i] = gBuffers[i].shaderResourceView.Get();
		}

		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, gBufferCount, shaderResources);

		std::shared_ptr<const Shader> shader = myShaderManager->GetShader(eShaderType::Deferred);
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
		shaderResources[0] = myRenderTargetManager->GetRenderTargets(aRenderTargetType)[0].shaderResourceView.Get();
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, shaderResources);

		const std::shared_ptr<const Graphics::Shader> shader = myShaderManager->GetShader(Graphics::eShaderType::PostProcessing);
		shader->BindThisShader(myContext.Get());

		RenderFullScreenQuad();

		ID3D11ShaderResourceView* nullViews = nullptr;
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
	}

	void GraphicsEngine::ApplyBloom()
	{
		if (myLightManager->GetPostProcessData()->useBloom == false)
		{
			SetRenderTarget(eRenderTargetType::Bloom);
			RenderFullScreenCopy(eRenderTargetType::Deferred);
			return;
		}

		FilterPixelForBloom();

		myStateManager->SetSamplerState(myContext, eSamplerState::Trilinear_Clamp);

		DownAndUpSampleForBloom();
		RenderBloom();

		myStateManager->SetBlendState(myContext, eBlendState::Disabled);
		myStateManager->SetSamplerState(myContext, eSamplerState::Bilinear_Warp);
	}

	void GraphicsEngine::RenderFullScreenCopy(const eRenderTargetType aRenderTargetType)
	{
		ID3D11ShaderResourceView* shaderResources[1] = {};
		shaderResources[0] = myRenderTargetManager->GetRenderTargets(aRenderTargetType)[0].shaderResourceView.Get();
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, shaderResources);

		const std::shared_ptr<const Graphics::Shader> shader = myShaderManager->GetShader(Graphics::eShaderType::Copy);
		shader->BindThisShader(myContext.Get());

		RenderFullScreenQuad();

		ID3D11ShaderResourceView* nullViews = nullptr;
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
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

		ID3D11RenderTargetView* backBuffer = myRenderTargetManager->GetRenderTargets(eRenderTargetType::Backbuffer)[0].renderTargetView.Get();
		backBuffer->Release();
		myRenderTargetManager->ReleaseDepthBuffer();

		[[maybe_unused]] const HRESULT result = mySwapChain->ResizeBuffers(0, newWindowSize.x, newWindowSize.y, DXGI_FORMAT_UNKNOWN, 0);
		assert(SUCCEEDED(result) && "Failed to resize buffer");

		{ //TO-DO(v10.0.5): Figure a out to resize buffers properly
			myRenderTargetManager->ReInit(myDevice, mySwapChain, myContext, newWindowSize);
		}
	}

	void GraphicsEngine::SetCamera(Graphics::Camera* aCamera)
	{
		myCurrentCameraRaw = aCamera;
		myConstantBufferManager->UpdateCameraConstantBuffer(myCurrentCameraRaw, Global::GetResolution());
	}

	void GraphicsEngine::SetRenderTarget(eRenderTargetType aRenderTargetType, const bool aUseDepthBuffer)
	{
		myRenderTargetManager->SetRenderTarget(myContext, aRenderTargetType, aUseDepthBuffer);
	}

	void GraphicsEngine::SetToDefaultCamera()
	{
		myCurrentCameraRaw = myEditorCamera.get();
	}

	Drawer::Renderer* GraphicsEngine::GetRenderer()
	{
		return myRenderer.get();
	}

	ModelFactory* GraphicsEngine::GetModelFactory()
	{
		return myModelFactory.get();
	}

	const Drawer::Renderer* GraphicsEngine::GetRenderer() const
	{
		return myRenderer.get();
	}

	const ModelFactory* GraphicsEngine::GetModelFactory() const
	{
		return myModelFactory.get();
	}

	Graphics::Camera* GraphicsEngine::GetCurrentCamera()
	{
		return myCurrentCameraRaw;
	}

	std::shared_ptr<Graphics::Camera> GraphicsEngine::GetEditorCamera()
	{
#ifndef _SIMPLE
		return myEditorCamera;
#else
		return myDefaultCamera;
#endif
	}

	const std::shared_ptr<Camera> GraphicsEngine::GetEditorCamera() const
	{
#ifndef _SIMPLE
		return myEditorCamera;
#else
		return myDefaultCamera;
#endif
	}

	const Graphics::Camera* GraphicsEngine::GetCurrentCamera() const
	{
		return myCurrentCameraRaw;
	}

	ConstantBufferManager* GraphicsEngine::GetConstantBufferManager()
	{
		return myConstantBufferManager.get();
	}

	RenderTargetManager* GraphicsEngine::GetRenderTargetManager()
	{
		return myRenderTargetManager.get();
	}

	GenericDataManager* GraphicsEngine::GetGenericDataManager()
	{
		return myGenericDataManager.get();
	}

	ShaderManager* GraphicsEngine::GetShaderManager()
	{
		return myShaderManager.get();
	}

	StateManager* GraphicsEngine::GetStateManager()
	{
		return myStateManager.get();
	}

	LightManager* GraphicsEngine::GetLightManager()
	{
		return myLightManager.get();
	}

	TextureManager* GraphicsEngine::GetTextureManager()
	{
		return myTextureManager.get();
	}

	ComPtr<ID3D11Device> GraphicsEngine::GetDevice()
	{
		return myDevice;
	}

	ComPtr<ID3D11DeviceContext> GraphicsEngine::GetContext()
	{
		return myContext;
	}

	void GraphicsEngine::CreateSwapChain(HWND& aWindowHandle, const Math::Vector2ui aSize)
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = aSize.x;
		swapChainDesc.BufferDesc.Height = aSize.y;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
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

		[[maybe_unused]] const HRESULT result = D3D11CreateDeviceAndSwapChain(
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
}
