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
		myStateManager = std::make_unique<StateManager>();
		myShaderManager = std::make_unique<ShaderManager>();
		myConstantBufferManager = std::make_unique<ConstantBufferManager>();
		myTextureManager = std::make_unique<TextureManager>();
		myLightManager = std::make_unique<LightManager>();
		myImGuiEngine = std::make_unique<Simple::ImGuiEngine>();
		myViewPort = std::make_shared<D3D11_VIEWPORT>();
		myEditorCamera = std::make_shared<Graphics::Camera>();
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

		myGenericDataManager->Init();
		myStateManager->Init(myDevice);
		myShaderManager->Init(myDevice);
		myConstantBufferManager->Init();
		myTextureManager->Init();
		myLightManager->Init();
		myEditorCamera->Init();
		myImGuiEngine->Init();
		myRenderer->Init();
		myModelFactory->Init();

		PreloadShaders();

		myStateManager->SetRasterizerState(myContext, eRasterizerState::BackfaceCulling);
		myStateManager->SetDepthStencilState(myContext, eDepthStencilState::Less_Equal);
		myStateManager->SetSamplerState(myContext, eSamplerState::Bilinear_Warp);
		myContext->RSSetViewports(1, myViewPort.get());

		myCurrentCameraRaw = myEditorCamera.get();
	}

	void GraphicsEngine::PrepareFrame()
	{
		ClearDepthStencilView();
		ClearAllRenderTargets();
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

	void GraphicsEngine::ClearAllRenderTargets()
	{
		for (size_t i = 0; i < static_cast<size_t>(eRenderTargetType::Count); ++i)
		{
			ClearRenderTarget(static_cast<eRenderTargetType>(i));
		}
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

		myStateManager->SetBlendState(myContext, eBlendState::Disabled);

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

			const std::shared_ptr<const Graphics::Shader> gaussianBlurShader = myShaderManager->GetShader(Graphics::eShaderType::GaussianBlur);
			gaussianBlurShader->BindThisShader(myContext.Get());

			RenderFullScreenQuad();

			ID3D11ShaderResourceView* nullViews = nullptr;
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
		}

		myStateManager->SetBlendState(myContext, eBlendState::AlphaBlend);

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

			const std::shared_ptr<const Graphics::Shader> shader = myShaderManager->GetShader(Graphics::eShaderType::GaussianBlur);
			shader->BindThisShader(myContext.Get());

			RenderFullScreenQuad();

			ID3D11ShaderResourceView* nullViews = nullptr;
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
		}

		myStateManager->SetBlendState(myContext, eBlendState::Disabled);

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

			const std::shared_ptr<const Graphics::Shader> shader = myShaderManager->GetShader(Graphics::eShaderType::Bloom);
			shader->BindThisShader(myContext.Get());

			RenderFullScreenQuad();

			ID3D11ShaderResourceView* nullViews[2] = { nullptr };
			myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 2, nullViews);
		}
	}

	void GraphicsEngine::ClearRenderTarget(const eRenderTargetType aRenderTargetType)
	{
		const std::vector<RenderTarget>& renderTargets = myRenderTargets[static_cast<size_t>(aRenderTargetType)];
		const std::array<float, 4>& clearColor = myGenericDataManager->GetClearColor();

		for (size_t i = 0; i < renderTargets.size(); ++i)
		{
			myContext->ClearRenderTargetView(renderTargets[i].renderTargetView.Get(), &clearColor[0]);
		}
	}

	void GraphicsEngine::ClearDepthStencilView()
	{
		myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
		shaderResources[0] = GetRenderTargets(aRenderTargetType)[0].shaderResourceView.Get();
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
		shaderResources[0] = GetRenderTargets(aRenderTargetType)[0].shaderResourceView.Get();
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, shaderResources);

		const std::shared_ptr<const Graphics::Shader> shader = myShaderManager->GetShader(Graphics::eShaderType::Copy);
		shader->BindThisShader(myContext.Get());

		RenderFullScreenQuad();

		ID3D11ShaderResourceView* nullViews = nullptr;
		myContext->PSSetShaderResources(Global_StartSlot_GBuffer, 1, &nullViews);
	}

	void GraphicsEngine::UnbindAllRenderTargets()
	{
		static constexpr size_t maxRenderTargetSupportedByDX11 = 8;
		ID3D11RenderTargetView* nullViews[maxRenderTargetSupportedByDX11] = { nullptr };
		myContext->OMSetRenderTargets(maxRenderTargetSupportedByDX11, nullViews, nullptr);
	}

	void GraphicsEngine::UpdatePointlights(const size_t aLightIndex)
	{
		myConstantBufferManager->UpdatePointlights(aLightIndex, myLightManager->GetPointLightBufferData());
	}

	void GraphicsEngine::UpdateLightBuffer()
	{
		myConstantBufferManager->UpdateLightConstantBuffer(myLightManager->GetLightBufferData());
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

		[[maybe_unused]] const HRESULT result = mySwapChain->ResizeBuffers(0, newWindowSize.x, newWindowSize.y, DXGI_FORMAT_UNKNOWN, 0);
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

	void GraphicsEngine::SetCamera(Graphics::Camera* aCamera)
	{
		myCurrentCameraRaw = aCamera;
		myConstantBufferManager->UpdateCameraConstantBuffer(myCurrentCameraRaw, Global::GetResolution());
	}

	void GraphicsEngine::SetToDefaultCamera()
	{
		myCurrentCameraRaw = myEditorCamera.get();
	}

	std::vector<RenderTarget>& GraphicsEngine::GetRenderTargets(const eRenderTargetType aRenderTargetType)
	{
		return myRenderTargets[static_cast<size_t>(aRenderTargetType)];
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
		return myEditorCamera;
	}

	const Graphics::Camera* GraphicsEngine::GetCurrentCamera() const
	{
		return myCurrentCameraRaw;
	}

	const std::shared_ptr<Camera> GraphicsEngine::GetEditorCamera() const
	{
		return myEditorCamera;
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

	ComPtr<ID3D11ShaderResourceView> GraphicsEngine::GetShaderResourceView(const eRenderTargetType aRenderTargetType, const size_t aIndex)
	{
		return myRenderTargets[static_cast<size_t>(aRenderTargetType)][aIndex].shaderResourceView;
	}

	ComPtr<ID3D11DepthStencilView> GraphicsEngine::GetDepthBuffer()
	{
		return myDepthBuffer;
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
}
