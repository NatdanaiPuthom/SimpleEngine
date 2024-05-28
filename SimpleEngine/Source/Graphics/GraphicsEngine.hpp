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
		ComPtr<ID3D11DepthStencilState> myOnlyGreaterDepth;
		ComPtr<ID3D11DepthStencilState> myDepthStencilState;
		ComPtr<ID3D11BlendState> myBlendStateDisable;
		ComPtr<ID3D11BlendState> myBlendAdditive;

		void CreateBlendState();

		GraphicsEngine();
		~GraphicsEngine();

		const bool Init(HWND& aWindowHandle, const Math::Vector2ui& aWindowSize);

		bool BeginFrame();
		void EndFrame();

		void RenderDeferredFromGBuffer();

		void AddPointLight(const PointLightData& aPointLightData);
		PointLightData GetPointLightData(const int aIndex);

		const bool AddTexture(const char* aFileName, const unsigned int aSlot = 0);
		const bool AddShader(const char* aPSFile, const char* aVSFile);

		bool IsVSyncActive() const;
	public:
		void SetGlobalGraphicsEngineToThis();
		void SetDirectionalLightDirection(const Math::Vector3f& aDirection);
		void SetDirectionalLightColor(const Math::Vector4f& aColor);
		void SetAmbientLightColorAndIntensity(const Math::Vector4f& aColorAndIntensity);
		void SetVSync(const bool aShouldTurnOn);
		void SetFPSLevelCap(const unsigned int aCapLevel);
		void SetRasterizerState(const eRasterizerState aRasterizerState);

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

		unsigned int GetFPSLevelCap() const;
	private:
		void CreateViewport(const Math::Vector2ui aSize);
		void CreateSwapChain(HWND& aWindowHandle, const Math::Vector2ui aSize);
		void CreateDepthBuffer(const Math::Vector2ui aSize);
		void CreateDepthStencilState();
		void CreateBackBuffer();
		void CreateSamplerState();
		void CreateCameraBuffer();
		void CreateTimeBuffer();
		void CreateLightBuffer();
		void CreateRasterizerStates();
		void CreateBonesBuffer();
		void CreateGBuffer(const Math::Vector2ui aResolution);
		void CreateDeferredBuffer(const Math::Vector2ui aResolution);
		std::vector<RenderTarget> CreateRenderTargets(const size_t aRenderTargetCount, DXGI_FORMAT* aArrayOfFormats, const Math::Vector2ui& aResolution);
	private:
		void LoadSettingsFromJson();
		void PrepareFrame();
		void PreloadTextures();
		void PreloadShaders();
		void ClearLightBuffer();
		void ClearGBuffer();
		void ClearDepthStencilView();
		void UnbindAllRenderTargets();
		void UpdateCameraBuffer();
		void UpdateLightBuffer();
	private:
		std::unordered_map<std::string, const std::shared_ptr<const Texture>> myLoadedTextures;
		std::unordered_map<std::pair<std::string, std::string>, std::shared_ptr<const Shader>, SimpleUtilities::PairHash, SimpleUtilities::PairEqual> myLoadedShaders;
		std::array<std::vector<RenderTarget>, static_cast<size_t>(eRenderTargetType::Count)> myRenderTargets;
		std::array<ComPtr<ID3D11RasterizerState>, static_cast<size_t>(eRasterizerState::Count)> myRasterizerStates;
		std::array<float, 4> myClearColor;

		ComPtr<ID3D11Device> myDevice;
		ComPtr<ID3D11DeviceContext> myContext;
		ComPtr<IDXGISwapChain> mySwapChain;

		ComPtr<ID3D11DepthStencilView> myDepthBuffer;
		

		ComPtr<ID3D11SamplerState> mySamplerState;
		ComPtr<ID3D11RasterizerState> myCurrentRasterizerState;

		std::shared_ptr<Camera> myCurrentCamera;
		std::shared_ptr<Camera> myEditorCamera;
		std::shared_ptr<Camera> myShadowCamera;

		std::shared_ptr<const D3D11_VIEWPORT> myViewPort;

		std::unique_ptr<ConstantBuffer> myCameraConstantBuffer;
		std::unique_ptr<ConstantBuffer> myTimeConstantBuffer;
		std::unique_ptr<ConstantBuffer> myLightConstantBuffer;
		std::unique_ptr<ConstantBuffer> myJointsConstantBuffer;

		std::unique_ptr<LightBufferData> myLightBufferData;

		std::unique_ptr<ModelFactory> myModelFactory;
		std::unique_ptr<Drawer::Renderer> myRenderer;
		std::unique_ptr<Simple::ImGuiEngine> myImGuiEngine;

		unsigned int myFPSLevelCap;
		bool myVSync;
	};
}