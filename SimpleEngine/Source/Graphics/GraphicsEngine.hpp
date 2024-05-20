#pragma once
#include "Engine/SimpleUtilities/HashStuff.hpp"
#include "Graphics/BufferData.hpp"
#include "Graphics/GraphicsDeclarations.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include <unordered_map>
#include <memory>
#include <array>
#include <string>

namespace Test
{
	struct ShadowRTV
	{
		ComPtr<ID3D11RenderTargetView> rtv;
		ComPtr<ID3D11ShaderResourceView> srv;

		void Clear(ComPtr<ID3D11DeviceContext> aContext)
		{
			Math::Vector4f clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
			aContext->ClearRenderTargetView(rtv.Get(), &clearColor.x);
		}
	};

	struct ShadowDSV
	{
		ComPtr<ID3D11DepthStencilView> dsv;
		ComPtr<ID3D11ShaderResourceView> srv;

		void Clear(ComPtr<ID3D11DeviceContext> aContext)
		{
			aContext->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		}
	};
}

namespace Simple
{
	class ImGuiEngine;
}

namespace Graphics
{
	class ConstantBuffer;
}

#undef max

namespace Graphics
{
	class GraphicsEngine final
	{
	public:
		void SetShadowCamera(std::shared_ptr<Camera> aCamera)
		{
			myShadowCamera = aCamera;
		}

		Test::ShadowDSV CreateShadowDSV(const Math::Vector2ui& aResolution)
		{
			HRESULT result;

			D3D11_TEXTURE2D_DESC desc = { 0 };
			desc.Width = aResolution.x;
			desc.Height = aResolution.y;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			ID3D11Texture2D* texture;
			result = myDevice->CreateTexture2D(&desc, nullptr, &texture);
			assert(SUCCEEDED(result));

			ID3D11DepthStencilView* DSV;
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

			dsvDesc.Flags = 0;
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			result = myDevice->CreateDepthStencilView(texture, &dsvDesc, &DSV);
			assert(SUCCEEDED(result));

			Test::ShadowDSV textureResult;
			textureResult.dsv = DSV;
			DSV->Release();

			ID3D11ShaderResourceView* SRV;
			D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
			srDesc.Format = DXGI_FORMAT_R32_FLOAT;
			srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srDesc.Texture2D.MostDetailedMip = 0;
			srDesc.Texture2D.MipLevels = std::numeric_limits<UINT>::max();

			result = myDevice->CreateShaderResourceView(texture, &srDesc, &SRV);
			assert(SUCCEEDED(result));

			textureResult.srv = SRV;

			SRV->Release();
			texture->Release();

			return textureResult;
		}

		Test::ShadowRTV CreateShadowRTV(const Math::Vector2ui& aResolution, const DXGI_FORMAT aFormat)
		{
			HRESULT result;

			D3D11_TEXTURE2D_DESC desc = { 0 };
			desc.Width = aResolution.x;
			desc.Height = aResolution.y;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = aFormat;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			ID3D11Texture2D* texture;
			result = myDevice->CreateTexture2D(&desc, nullptr, &texture);
			assert(SUCCEEDED(result));

			Test::ShadowRTV textureResult;

			ID3D11RenderTargetView* RTV;
			result = myDevice->CreateRenderTargetView(texture, nullptr, &RTV);
			assert(SUCCEEDED(result));

			textureResult.rtv = RTV;
			RTV->Release();

			ID3D11ShaderResourceView* SRV;
			result = myDevice->CreateShaderResourceView(texture, nullptr, &SRV);
			assert(SUCCEEDED(result));
			textureResult.srv = SRV;

			SRV->Release();
			texture->Release();

			return textureResult;
		}


		GraphicsEngine();
		~GraphicsEngine();

		const bool Init(const Math::Vector2ui& aWindowSize, HWND& aWindowHandle);

		bool BeginFrame();
		void EndFrame();

		const bool AddTexture(const char* aFileName, const unsigned int aSlot = 0);
		const bool AddShader(const char* aPSFile, const char* aVSFile);

		bool IsVSyncActive() const;
	public:
		void SetGlobalGraphicsEngineToThis();
		void SetDirectionalLightDirection(const Math::Vector3f& aDirection);
		void SetDirectionalLightColor(const Math::Vector4f& aColor);
		void SetVSync(const bool aShouldTurnOn);
		void SetFPSLevelCap(const unsigned int aCapLevel);
		void SetRasterizerState(const eRasterizerState aRasterizerState);

		//NOTE(v6.0.0?): Call Global::SetWindowSize() instead
		void SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen);

		void SetRenderTarget(eRenderTarget aRenderTarget);
		void SetCamera(std::shared_ptr<Camera> aCamera);
		void SetToDefaultCamera();
	public:
		ComPtr<ID3D11Device> GetDevice();
		ComPtr<ID3D11DeviceContext> GetContext();
		ComPtr<ID3D11ShaderResourceView> GetShaderResourceView(const eRenderTarget aRenderTarget);

		std::shared_ptr<Camera> GetCurrentCamera();
		std::shared_ptr<Camera> GetEditorCamera();
		std::shared_ptr<Camera> GetShadowCamera();

		//NOTE(v9.35.5): Will Add and cache the texture if it does not already exist
		std::shared_ptr<const Texture> GetTexture(const char* aFilePath);
		std::shared_ptr<const Texture> GetTexture(const eTextureType aTextureType);

		//NOTE(v9.36.1): Will Add and cache the shader if it does not already exist
		std::shared_ptr<const Shader> GetShader(const char* aPSFile, const char* aVSFile);
		std::shared_ptr<const Shader> GetShader(const eShaderType aShaderType);

		Drawer::Renderer* GetRenderer();
		ModelFactory* GetModelFactory();

		Math::Vector4f GetDirectionalLightColor() const;
		Math::Vector3f GetDirectionalLightDirection() const;

		unsigned int GetFPSLevelCap() const;
	private:
		void CreateViewport(const int aWidth, const int aHeight);
		void CreateSwapChain(HWND& aWindowHandle, const int aWidth, const int aHeight);
		void CreateDepthBuffer(const int aWidth, const int aHeight);
		void CreateDepthStencilState();
		void CreateBackBuffer();
		void CreateFrameBuffer();
		void CreateSamplerState();
		void CreateCameraBuffer();
		void CreateTimeBuffer();
		void CreateLightBuffer();
		void CreateRenderTarget(RenderTarget* aRenderTarget, const int aWidth, const int aHeight, const DXGI_FORMAT aFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
		void CreateRasterizerStates();
		void CreateBonesBuffer();
	private:
		void PrepareFrame();
		void LoadSettingsFromJson();
		void PreloadTextures();
		void PreloadShaders();
		void UpdateCameraBuffer();
	private:
		std::unordered_map<std::string, const std::shared_ptr<const Texture>> myLoadedTextures;
		std::unordered_map<std::pair<std::string, std::string>, std::shared_ptr<const Shader>, SimpleUtilities::PairHash, SimpleUtilities::PairEqual> myLoadedShaders;
		std::array<ComPtr<ID3D11RasterizerState>, static_cast<size_t>(eRasterizerState::Count)> myRasterizerStates;
		std::array<RenderTarget, static_cast<size_t>(eRenderTarget::Count)> myRenderTargets;

		ComPtr<ID3D11Device> myDevice;
		ComPtr<ID3D11DeviceContext> myContext;
		ComPtr<IDXGISwapChain> mySwapChain;

		ComPtr<ID3D11DepthStencilView> myDepthBuffer;
		ComPtr<ID3D11DepthStencilState> myDepthStencilState;

		ComPtr<ID3D11Buffer> myFrameBuffer;
		ComPtr<ID3D11Buffer> myObjectBuffer;

		ComPtr<ID3D11SamplerState> mySamplerState;
		ComPtr<ID3D11RasterizerState> myCurrentRasterizerState;

		std::shared_ptr<Camera> myCurrentCamera;
		std::shared_ptr<Camera> myEditorCamera;
		std::shared_ptr<Camera> myShadowCamera;
		std::shared_ptr<const D3D11_VIEWPORT> myViewPort;

		std::unique_ptr<ConstantBuffer> myCameraConstantBuffer;
		std::unique_ptr<ConstantBuffer> myTimeConstantBuffer;
		std::unique_ptr<ConstantBuffer> myLightConstantBuffer;
		std::unique_ptr<ConstantBuffer> myBonesConstantBuffer;

		std::unique_ptr<LightBufferData> myLightBufferData;

		std::unique_ptr<ModelFactory> myModelFactory;
		std::unique_ptr<Drawer::Renderer> myRenderer;
		std::unique_ptr<Simple::ImGuiEngine> myImGuiEngine;

		float myClearColor[4];
		unsigned int myFPSLevelCap;
		bool myVSync;
	};
}