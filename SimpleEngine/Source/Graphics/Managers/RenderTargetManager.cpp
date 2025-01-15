#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "RenderTargetManager.hpp"

namespace Graphics
{
	RenderTargetManager::RenderTargetManager()
	{

	}

	RenderTargetManager::~RenderTargetManager()
	{
	}

	void RenderTargetManager::Init(
		Microsoft::WRL::ComPtr<ID3D11Device> aDevice, 
		Microsoft::WRL::ComPtr<IDXGISwapChain> aSwapChain,
		const Math::Vector2ui& aResolution)
	{
		CreateBackBuffer(aDevice, aSwapChain);
		CreateDepthBuffer(aDevice, aResolution);
		CreateGRenderTarget(aDevice, aResolution);
		CreateDeferredRenderTarget(aDevice, aResolution);
		CreatePostProcessingRenderTarget(aDevice, aResolution);
		CreateBloomDownAndUpSampleRenderTarget(aDevice, aResolution);
		CreateBloomRenderTarget(aDevice, aResolution);
	}

	void RenderTargetManager::UnbindAllRenderTargets(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext)
	{
		static constexpr size_t maxRenderTargetSupportedByDX11 = 8;
		ID3D11RenderTargetView* nullViews[maxRenderTargetSupportedByDX11] = { nullptr };
		aContext->OMSetRenderTargets(maxRenderTargetSupportedByDX11, nullViews, nullptr);
	}

	void RenderTargetManager::ClearRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext, const eRenderTargetType aRenderTargetType, const std::array<float, 4>& aClearColor)
	{
		const std::vector<RenderTarget>& renderTargets = myRenderTargets[static_cast<size_t>(aRenderTargetType)];

		for (size_t i = 0; i < renderTargets.size(); ++i)
		{
			aContext->ClearRenderTargetView(renderTargets[i].renderTargetView.Get(), &aClearColor[0]);
		}
	}

	void RenderTargetManager::ClearDepthStencilView(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext)
	{
		aContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void RenderTargetManager::ClearAllRenderTargets(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext, const std::array<float, 4>& aClearColor)
	{
		for (size_t i = 0; i < static_cast<size_t>(eRenderTargetType::Count); ++i)
		{
			ClearRenderTarget(aContext, static_cast<eRenderTargetType>(i), aClearColor);
		}
	}

	void RenderTargetManager::ReleaseDepthBuffer()
	{
		myDepthBuffer->Release();
	}

	std::vector<RenderTarget> RenderTargetManager::CreateRenderTargets(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const size_t aRenderTargetCount, DXGI_FORMAT* aArrayOfFormats, const Math::Vector2ui& aResolution)
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

			HRESULT result = aDevice->CreateTexture2D(&desc, nullptr, &texture);
			assert(SUCCEEDED(result) && "Failed to create Texture2D");

			result = aDevice->CreateShaderResourceView(texture, nullptr, renderTarget.shaderResourceView.GetAddressOf());
			assert(SUCCEEDED(result) && "Failed to create ShaderResourceView");

			result = aDevice->CreateRenderTargetView(texture, nullptr, renderTarget.renderTargetView.GetAddressOf());
			assert(SUCCEEDED(result) && "Failed to create RenderTargetView");

			texture->Release();

			renderTargets[i] = renderTarget;
		}

		return renderTargets;
	}

	void RenderTargetManager::CreateBackBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, Microsoft::WRL::ComPtr<IDXGISwapChain> aSwapChain)
	{
		if (myRenderTargets[static_cast<size_t>(eRenderTargetType::Backbuffer)].empty())
		{
			myRenderTargets[static_cast<size_t>(eRenderTargetType::Backbuffer)].resize(1);
		}

		ID3D11Texture2D* backBufferTexture = nullptr;

		HRESULT result = aSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
		assert(SUCCEEDED(result) && "Failed to get Backbuffer");

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		result = aDevice->CreateRenderTargetView(backBufferTexture, &rtvDesc, myRenderTargets[static_cast<size_t>(eRenderTargetType::Backbuffer)][0].renderTargetView.GetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create Backbuffer");

		backBufferTexture->Release();
	}

	void RenderTargetManager::CreateDepthBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution)
	{
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = aResolution.x;
		descDepth.Height = aResolution.y;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ComPtr<ID3D11Texture2D> pDepthStencil;

		HRESULT result = aDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
		assert(SUCCEEDED(result) && "Failed to create DepthStencilTexture");

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		descDSV.Flags = 0;

		result = aDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, myDepthBuffer.GetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create DepthStencilView");
	}

	void RenderTargetManager::CreateBloomRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution)
	{
		std::array<DXGI_FORMAT, 1> formats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT
		};

		myRenderTargets[static_cast<size_t>(eRenderTargetType::Bloom)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::Bloom)] = CreateRenderTargets(aDevice, formats.size(), &formats[0], aResolution);
	}

	void RenderTargetManager::CreateBloomDownAndUpSampleRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution)
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

			HRESULT result = aDevice->CreateTexture2D(&desc, nullptr, &texture);
			assert(SUCCEEDED(result) && "Failed to create Texture2D");

			result = aDevice->CreateShaderResourceView(texture, nullptr, renderTarget.shaderResourceView.GetAddressOf());
			assert(SUCCEEDED(result) && "Failed to create ShaderResourceView");

			result = aDevice->CreateRenderTargetView(texture, nullptr, renderTarget.renderTargetView.GetAddressOf());
			assert(SUCCEEDED(result) && "Failed to create RenderTargetView");

			texture->Release();

			renderTargets[i] = renderTarget;
		}

		myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::BloomDownAndUpScale)] = renderTargets;
	}

	void RenderTargetManager::CreatePostProcessingRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution)
	{
		std::array<DXGI_FORMAT, 1> formats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT
		};

		myRenderTargets[static_cast<size_t>(eRenderTargetType::PostProcessing)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::PostProcessing)] = CreateRenderTargets(aDevice, formats.size(), &formats[0], aResolution);
	}

	void RenderTargetManager::CreateDeferredRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution)
	{
		std::array<DXGI_FORMAT, 1> formats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT
		};

		myRenderTargets[static_cast<size_t>(eRenderTargetType::Deferred)] = std::vector<RenderTarget>();
		myRenderTargets[static_cast<size_t>(eRenderTargetType::Deferred)] = CreateRenderTargets(aDevice, formats.size(), &formats[0], aResolution);
	}

	void RenderTargetManager::CreateGRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution)
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
		myRenderTargets[static_cast<size_t>(eRenderTargetType::GBuffer)] = CreateRenderTargets(aDevice, formats.size(), &formats[0], aResolution);
	}

	void RenderTargetManager::SetRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext, eRenderTargetType aRenderTargetType, const bool aUseDepthBuffer)
	{
		UnbindAllRenderTargets(aContext);

		const std::vector<RenderTarget>& originalRenderTargets = myRenderTargets[static_cast<size_t>(aRenderTargetType)];
		const size_t count = originalRenderTargets.size();

		std::vector<ID3D11RenderTargetView*> renderTargetsPointer(count);

		for (size_t i = 0; i < count; ++i)
		{
			renderTargetsPointer[i] = originalRenderTargets[i].renderTargetView.Get();
		}

		ID3D11DepthStencilView* depthBuffer = nullptr;

		if (aUseDepthBuffer)
		{
			depthBuffer = myDepthBuffer.Get();
		}

		aContext->OMSetRenderTargets(static_cast<unsigned int>(count), &renderTargetsPointer[0], depthBuffer);
	}

	std::vector<RenderTarget>& RenderTargetManager::GetRenderTargets(const eRenderTargetType aRenderTargetType)
	{
		return myRenderTargets[static_cast<size_t>(aRenderTargetType)];
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderTargetManager::GetShaderResourceView(const eRenderTargetType aRenderTargetType, const size_t aIndex)
	{
		return myRenderTargets[static_cast<size_t>(aRenderTargetType)][aIndex].shaderResourceView;
	}
}
