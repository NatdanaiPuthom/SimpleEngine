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

namespace Simple
{
	class ImGuiEngine;
}

namespace Graphics
{
	class ConstantBuffer;
}

namespace Graphics
{
	class GraphicsEngine final
	{
	public:
		GraphicsEngine();
		~GraphicsEngine();

		const bool Init(HWND& aWindowHandle, const Math::Vector2ui& aWindowSize);

		bool BeginFrame();
		void EndFrame();

		bool IsVSyncActive() const;

		//NOTE(v10.0.4): I have no clue what I am doing but it works for now
		void ApplyAmbientAndDirectionalLightDeferred(const eRenderTargetType aRenderTargetType);

		//NOTE(v10.0.4): I have no clue what I am doing but it works for now
		void RenderFullScreenQuad();

		//NOTE(v10.0.4): I have no clue what I am doing but it works for now
		void RenderFullScreenCopy(const eRenderTargetType aRenderTargetType);

		//NOTE(v10.0.4): I have no clue what I am doing but it works for now
		void ApplyPostProcessing(const eRenderTargetType aRenderTargetType);
	
		//NOTE(v10.0.4): I have no clue what I am doing but it works for now
		void AddPointLight(const PointLightData& aPointLightData);

		const bool AddTexture(const char* aFileName, const unsigned int aSlot = 0);
		const bool AddShader(const char* aPSFile, const char* aVSFile);

		//TO-DO(v10.0.3): Fix this properly. This quick fix right now
		void UpdateLightBuffer(const size_t aLightIndex);

	public:
		void SetGlobalGraphicsEngineToThis();
		void SetDirectionalLightDirection(const Math::Vector3f& aDirection);
		void SetDirectionalLightColor(const Math::Vector4f& aColor);
		void SetAmbientLightColorAndIntensity(const Math::Vector4f& aColorAndIntensity);
		void SetSaturation(const float aValue);
		void SetExposure(const float aValue);
		void SetContrast(const float aValue);
		void SetBlackPoint(const float aValue);
		void SetTint(const Math::Vector3f& aColor);
		void SetVSync(const bool aShouldTurnOn);
		void SetFPSLevelCap(const unsigned int aCapLevel);

		//TO-DO(v10.0.4): Fix some of states not working since I switched to deferred rendering
		void SetRasterizerState(const eRasterizerState aRasterizerState);

		void SetBlendState(const eBlendState aBlendState);
		void SetDepthStencilState(const eDepthStencilState aDepthStencilState);

		//NOTE(v9.37.0?): Call SetWindowSizeNextFrame instead.
		void SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen);

		void SetRenderTarget(eRenderTargetType aRenderTargetType, ID3D11DepthStencilView* aDepthBuffer = nullptr);
		void SetCamera(std::shared_ptr<Camera> aCamera);
		void SetToDefaultCamera();
	public:
		std::vector<RenderTarget>& GetRenderTargets(const eRenderTargetType aRenderTargetType);

		ComPtr<ID3D11Device> GetDevice();
		ComPtr<ID3D11DeviceContext> GetContext();
		ComPtr<ID3D11ShaderResourceView> GetShaderResourceView(const eRenderTargetType aRenderTargetType, const size_t aIndex = 0);
		ComPtr<ID3D11DepthStencilView> GetDepthBuffer();

		const eRasterizerState GetCurrentRasterizerState() const;

		std::shared_ptr<Camera> GetCurrentCamera();
		std::shared_ptr<Camera> GetEditorCamera();
		std::shared_ptr<Camera> GetShadowCamera();

		//NOTE(v9.35.5): Will Add and cache the texture if it does not already exist. aSlot is used to set slot when adding texture
		std::shared_ptr<const Texture> GetTexture(const char* aFilePath);
		std::shared_ptr<const Texture> GetTexture(const eTextureType aTextureType);

		//NOTE(v9.36.1): Will Add and cache the shader if it does not already exist
		std::shared_ptr<const Shader> GetShader(const char* aPSFile, const char* aVSFile);
		std::shared_ptr<const Shader> GetShader(const eShaderType aShaderType);

		Drawer::Renderer* GetRenderer();
		ModelFactory* GetModelFactory();

		Math::Vector4f GetAmbientLightColorAndIntensity() const;
		Math::Vector4f GetDirectionalLightColor() const;
		Math::Vector3f GetDirectionalLightDirection() const;
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

		void CreateCameraConstantBuffer();
		void CreateTimeConstantBuffer();
		void CreateLightConstantBuffer();
		void CreatePostProcessingConstantBuffer();
		void CreateJointsConstantBuffer();

		void CreateGRenderTarget(const Math::Vector2ui aResolution);
		void CreateDeferredRenderTarget(const Math::Vector2ui aResolution);
		void CreatePostProcessingRenderTarget(const Math::Vector2ui aResolution);

		std::vector<RenderTarget> CreateRenderTargets(const size_t aRenderTargetCount, DXGI_FORMAT* aArrayOfFormats, const Math::Vector2ui& aResolution);
	private:
		void LoadSettingsFromJson();

		void PrepareFrame();
		void PreloadTextures();
		void PreloadShaders();

		void ClearPointLightCount();
		void ClearRenderTarget(const eRenderTargetType aRenderTargetType);
		void ClearDepthStencilView();

		void UnbindAllRenderTargets();

		void UpdateCameraConstantBuffer();
		void UpdateTimeConstantBuffer();
	private:
		std::unordered_map<std::string, const std::shared_ptr<const Texture>> myLoadedTextures;
		std::unordered_map<std::pair<std::string, std::string>, std::shared_ptr<const Shader>, SimpleUtilities::PairHash, SimpleUtilities::PairEqual> myLoadedShaders;
		std::array<std::vector<RenderTarget>, static_cast<size_t>(eRenderTargetType::Count)> myRenderTargets;
		std::array<ComPtr<ID3D11RasterizerState>, static_cast<size_t>(eRasterizerState::Count)> myRasterizerStates;
		std::array<ComPtr<ID3D11DepthStencilState>, static_cast<size_t>(eDepthStencilState::Count)> myDepthStencilStates;
		std::array<ComPtr<ID3D11BlendState>, static_cast<size_t>(eBlendState::Count)> myBlendStates;
		std::array<float, 4> myClearColor;

		PostProcessData myPostProcessData;

		ComPtr<ID3D11Device> myDevice;
		ComPtr<ID3D11DeviceContext> myContext;
		ComPtr<IDXGISwapChain> mySwapChain;

		ComPtr<ID3D11DepthStencilView> myDepthBuffer;
		
		ComPtr<ID3D11SamplerState> mySamplerState;

		std::shared_ptr<Camera> myCurrentCamera;
		std::shared_ptr<Camera> myEditorCamera;
		std::shared_ptr<Camera> myShadowCamera;

		std::shared_ptr<const D3D11_VIEWPORT> myViewPort;

		std::unique_ptr<ConstantBuffer> myCameraConstantBuffer;
		std::unique_ptr<ConstantBuffer> myTimeConstantBuffer;
		std::unique_ptr<ConstantBuffer> myJointsConstantBuffer;
		std::unique_ptr<ConstantBuffer> myLightConstantBuffer;
		std::unique_ptr<ConstantBuffer> myPostProcessConstantBuffer;

		std::unique_ptr<LightBufferData> myLightBufferData;

		std::unique_ptr<ModelFactory> myModelFactory;
		std::unique_ptr<Drawer::Renderer> myRenderer;
		std::unique_ptr<Simple::ImGuiEngine> myImGuiEngine;

		eRasterizerState myCurrentRasterizerState;

		unsigned int myFPSLevelCap;
		bool myVSync;
	};
}