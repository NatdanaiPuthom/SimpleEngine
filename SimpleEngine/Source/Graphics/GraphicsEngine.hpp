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
		void SetSkyColor(const Math::Vector4f& aColor);
		void SetGroundColor(const Math::Vector4f& aColor);
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
		ComPtr<ID3D11ShaderResourceView> GetImGuiShaderResourceView();
		ComPtr<ID3D11ShaderResourceView> GetWaterShaderResourceView();
		ComPtr<ID3D11ShaderResourceView> GetWaterRefractionShaderResourceView();

		std::shared_ptr<Camera> GetCurrentCamera();
		std::shared_ptr<Camera> GetEditorCamera();
		std::shared_ptr<const Texture> GetTexture(const char* aFilePath);
		std::shared_ptr<const Texture> GetDefaultTexture();
		std::shared_ptr<const Shader> GetDefaultShader();
		std::shared_ptr<const Shader> GetDefaultAnimatedShader();
		std::shared_ptr<const Shader> GetShader(const char* aPSFile, const char* aVSFile);

		Drawer::Renderer* GetRenderer();
		ModelFactory* GetModelFactory();

		Math::Vector4f GetDirectionalLightColor() const;
		Math::Vector3f GetDirectionalLightDirection() const;
		Math::Vector4f GetSkyColor() const;
		Math::Vector4f GetGroundColor() const;

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
		void CreateRenderTargetForImGuiImage(const int aWidth, const int aHeight);
		void CreateWaterRenderTarget(const int aWidth, const int aHeight);
		void CreateWaterRefractionRenderTarget(const int aWidth, const int aHeight);
		void CreateRasterizerStates();
		void CreateBonesBuffer();
	private:
		void PrepareFrame();
		void LoadSettingsFromJson();
		void PreloadTextures();
		void PreloadShaders();
	private:
		std::unordered_map<std::string, const std::shared_ptr<const Texture>> myLoadedTextures;
		std::unordered_map<std::pair<std::string, std::string>, std::shared_ptr<const Shader>, SimpleUtilities::PairHash, SimpleUtilities::PairEqual> myLoadedShaders;
		std::array<ComPtr<ID3D11RasterizerState>, static_cast<int>(eRasterizerState::Count)> myRasterizerStates;

		ComPtr<ID3D11Device> myDevice;
		ComPtr<ID3D11DeviceContext> myContext;
		ComPtr<IDXGISwapChain> mySwapChain;

		ComPtr<ID3D11RenderTargetView> myBackBuffer;
		ComPtr<ID3D11DepthStencilView> myDepthBuffer;
		ComPtr<ID3D11DepthStencilState> myDepthStencilState;

		ComPtr<ID3D11Buffer> myFrameBuffer;
		ComPtr<ID3D11Buffer> myObjectBuffer;

		ComPtr<ID3D11SamplerState> mySamplerState;
		ComPtr<ID3D11RasterizerState> myRasterizerState;

		std::shared_ptr<Camera> myCurrentCamera;
		std::shared_ptr<Camera> myEditorCamera;
		std::shared_ptr<const D3D11_VIEWPORT> myViewPort;

		std::unique_ptr<ConstantBuffer> myCameraConstantBuffer;
		std::unique_ptr<ConstantBuffer> myTimeConstantBuffer;
		std::unique_ptr<ConstantBuffer> myLightConstantBuffer;
		std::unique_ptr<ConstantBuffer> myBonesConstantBuffer;

		std::unique_ptr<LightBufferData> myLightBufferData;

		std::unique_ptr<RenderTarget> myWaterReflectionRenderTarget;
		std::unique_ptr<RenderTarget> myWaterRefractionRenderTarget;
		std::unique_ptr<RenderTarget> myImGuiImageRenderTarget;

		std::unique_ptr<ModelFactory> myModelFactory;
		std::unique_ptr<Drawer::Renderer> myRenderer;
		std::unique_ptr<Simple::ImGuiEngine> myImGuiEngine;

		float myClearColor[4];
		unsigned int myFPSLevelCap;
		bool myVSync;
	};
}