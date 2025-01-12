#pragma once
#include "Engine/SimpleUtilities/HashStuff.hpp"
#include "Graphics/BufferData.hpp"
#include "Graphics/GraphicsDeclarations.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include "Graphics/Texture/TextureManager.hpp"
#include "Graphics/DataContainer/GraphicsDataContainer.hpp"
#include <unordered_map>
#include <memory>
#include <array>
#include <string>

namespace Simple
{
	class ImGuiEngine;
}

namespace Graphics
{
	class ConstantBuffer;
	class ConstantBufferManager;
}

namespace Graphics
{
	class GraphicsEngine final
	{
	public:
		GraphicsEngine();
		~GraphicsEngine();

		void Init(HWND& aWindowHandle, const Math::Vector2ui& aWindowSize);

		bool BeginFrame();
		void EndFrame();

		void ApplyAmbientAndDirectionalLightDeferred(const eRenderTargetType aRenderTargetType);
		void ApplyPostProcessing(const eRenderTargetType aRenderTargetType);
		void ApplyBloom();

		void RenderFullScreenQuad();
		void RenderFullScreenCopy(const eRenderTargetType aRenderTargetType);

		void ClearAllRenderTargets();

		void AddPointLight(const PointLightData& aPointLightData);
		const bool AddShader(const char* aPSFile, const char* aVSFile);

		void UpdatePointlights(const size_t aLightIndex);
		void UpdateLightBuffer();

		bool IsVSyncActive() const;
	public:
		void SetToDefaultCamera();
		void SetBloom(const float aValue);
		void SetExposure(const float aValue);
		void SetContrast(const float aValue);
		void SetGlobalGraphicsEngineToThis();
		void SetSaturation(const float aValue);
		void SetBlackPoint(const float aValue);
		void SetVSync(const bool aShouldTurnOn);
		void SetTint(const Math::Vector3f& aColor);
		void SetUseBloom(const bool aShouldUseBloom);
		void SetBloomPixelThreshold(const float aValue);
		void SetCamera(Graphics::Camera* aCamera);
		void SetFPSLevelCap(const unsigned int aCapLevel);
		void SetBlendState(const eBlendState aBlendState);
		void SetSamplerState(const eSamplerState aSamplerState);
		void SetUseToneMapping(const bool aShouldUseToneMapping);
		void SetRasterizerState(const eRasterizerState aRasterizerState);
		void SetDepthStencilState(const eDepthStencilState aDepthStencilState);
		void SetRenderTarget(eRenderTargetType aRenderTargetType, ID3D11DepthStencilView* aDepthBuffer = nullptr);

		//NOTE(v9.37.0?): Call SetWindowSizeNextFrame instead.
		void SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen);
	public:
		std::vector<RenderTarget>& GetRenderTargets(const eRenderTargetType aRenderTargetType);

		ComPtr<ID3D11Device> GetDevice();
		ComPtr<ID3D11DeviceContext> GetContext();
		ComPtr<ID3D11ShaderResourceView> GetShaderResourceView(const eRenderTargetType aRenderTargetType, const size_t aIndex = 0);
		ComPtr<ID3D11DepthStencilView> GetDepthBuffer();

		const eRasterizerState GetCurrentRasterizerState() const;

		Graphics::Camera* GetCurrentCamera();
		std::shared_ptr<Camera> GetEditorCamera();

		const Graphics::Camera* GetCurrentCamera() const;
		const std::shared_ptr<Camera> GetEditorCamera() const;

		GraphicsDataContainer* GetDataContainer();
		TextureManager* GetTextureManager();

		//NOTE(v9.36.1): Will Add and cache the shader if it does not already exist
		std::shared_ptr<const Shader> GetShader(const char* aPSFile, const char* aVSFile);
		std::shared_ptr<const Shader> GetShader(const eShaderType aShaderType);

		Drawer::Renderer* GetRenderer();
		ModelFactory* GetModelFactory();
		const Drawer::Renderer* GetRenderer() const;
		const ModelFactory* GetModelFactory() const;

		PointLightData* GetPointLightDataArray() const;
		size_t GetPointLightCount() const;

		unsigned int GetFPSLevelCap() const;

		const PostProcessData& GetPostProcessData() const;

	private:
		void CreateViewport(const Math::Vector2ui aSize);

		void CreateSwapChain(HWND& aWindowHandle, const Math::Vector2ui aSize);
		void CreateBackBuffer();
		void CreateDepthBuffer(const Math::Vector2ui aSize);

		void CreateDepthStencilState();
		void CreateSamplerState();
		void CreateBlendStates();
		void CreateRasterizerStates();

		void CreateGRenderTarget(const Math::Vector2ui aResolution);
		void CreateDeferredRenderTarget(const Math::Vector2ui aResolution);
		void CreatePostProcessingRenderTarget(const Math::Vector2ui aResolution);
		void CreateBloomDownAndUpSampleRenderTarget(const Math::Vector2ui aResolution);
		void CreateBloomRenderTarget(const Math::Vector2ui aResolution);

		std::vector<RenderTarget> CreateRenderTargets(const size_t aRenderTargetCount, DXGI_FORMAT* aArrayOfFormats, const Math::Vector2ui& aResolution);
	private:
		void LoadSettingsFromJson();

		void PrepareFrame();
		void PreloadShaders();

		void FilterPixelForBloom();
		void DownAndUpSampleForBloom();
		void RenderBloom();

		void ClearPointLightCount();
		void ClearRenderTarget(const eRenderTargetType aRenderTargetType);
		void ClearDepthStencilView();

		void UnbindAllRenderTargets();
	private:
		std::unordered_map<std::pair<std::string, std::string>, std::shared_ptr<const Shader>, SimpleUtilities::PairHash, SimpleUtilities::PairEqual> myLoadedShaders;
		std::array<std::vector<RenderTarget>, static_cast<size_t>(eRenderTargetType::Count)> myRenderTargets;
		std::array<ComPtr<ID3D11RasterizerState>, static_cast<size_t>(eRasterizerState::Count)> myRasterizerStates;
		std::array<ComPtr<ID3D11DepthStencilState>, static_cast<size_t>(eDepthStencilState::Count)> myDepthStencilStates;
		std::array<ComPtr<ID3D11BlendState>, static_cast<size_t>(eBlendState::Count)> myBlendStates;
		std::array<ComPtr<ID3D11SamplerState>, static_cast<size_t>(eSamplerState::Count)> mySamplerStates;
		std::array<float, 4> myClearColor;

		PostProcessData myPostProcessData;

		ComPtr<ID3D11Device> myDevice;
		ComPtr<ID3D11DeviceContext> myContext;
		ComPtr<IDXGISwapChain> mySwapChain;

		ComPtr<ID3D11DepthStencilView> myDepthBuffer;

		std::shared_ptr<Camera> myEditorCamera;

		std::shared_ptr<const D3D11_VIEWPORT> myViewPort;

		std::unique_ptr<GraphicsDataContainer> myDataContainer;
		std::unique_ptr<ConstantBufferManager> myBufferManager;
		std::unique_ptr<TextureManager> myTextureManager;

		std::unique_ptr<PointLightBufferData> myPointLightBufferData;

		std::unique_ptr<ModelFactory> myModelFactory;
		std::unique_ptr<Drawer::Renderer> myRenderer;
		std::unique_ptr<Simple::ImGuiEngine> myImGuiEngine;

		Camera* myCurrentCameraRaw;

		eRasterizerState myCurrentRasterizerState;

		unsigned int myFPSLevelCap;
		bool myVSync;
	};
}