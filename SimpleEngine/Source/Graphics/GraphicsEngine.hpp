#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/SimpleUtilities/HashStuff.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include <unordered_map>
#include <memory>
#include <array>
#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

namespace Simple
{
	class ConstantBuffer;
	class Camera;
	class ImGuiEngine;
}

enum class eRasterizerState
{
	BackfaceCulling,
	NoFaceCulling,
	Wireframe,
	WireframeNoCulling,
	FrontFaceCulling,
	Count
};

enum class eRenderTarget
{
	Backbuffer,
	ImGui,
	WaterReflection,
	WaterRefraction,
};

struct alignas(16) FrameBufferData final
{
	Math::Matrix4x4f worldToClipMatrix;
	Math::Vector3f cameraPosition;
	const float paddingCameraPos = -1.0f;

	Math::Vector2ui resolution;
	const float paddingResolution[2] = { -1.0f };

	float waterHeight;
	const float paddingWaterHeight[3] = { -1.0f };

	float waterMoveFactor = 0.0f;
	const float paddingWaterMoveFactor[3] = { -1.0f };
};

struct alignas(16) ObjectBufferData final
{
	Math::Matrix4x4f modelWorldMatrix;
};

struct alignas(16) TimeBufferData final
{
	float totalTime = 0;
	const float padding[3] = { -1 };
};

struct alignas(16) LightBufferData final
{
	Math::Vector4f skyColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Math::Vector4f groundColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	Math::Vector3f directionalLightDirection;
	const float paddingDirectionalLightDirection = -1.0f;

	Math::Vector4f directionalLightColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	struct PointLightData
	{
		Math::Vector4f position;
		Math::Vector4f color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		float range = 0.0f;
		const float padding[3] = { -1.0f };
	} pointLights[8];

	unsigned int numberOfPointlights = 0;
	const float paddingPointlightData[3] = { -1.0f };
};

struct alignas(16) BonesBufferData
{
	Math::Matrix4x4f bonesTransform[SIMPLE_MAX_BONES];
};

struct RenderTarget final
{
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};

namespace Simple
{
	class GraphicsEngine final
	{
	public:
		GraphicsEngine();
		~GraphicsEngine();

		const bool Init(const Math::Vector2ui& aWindowSize, HWND& aWindowHandle);

		const bool BeginFrame();
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
		void SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen);
		void SetRenderTarget(eRenderTarget aRenderTarget);
		void SetCamera(std::shared_ptr<Simple::Camera> aCamera);
		void SetToDefaultCamera();
	public:
		ComPtr<ID3D11Device> GetDevice();
		ComPtr<ID3D11DeviceContext> GetContext();
		ComPtr<ID3D11ShaderResourceView> GetImGuiShaderResourceView();
		ComPtr<ID3D11ShaderResourceView> GetWaterShaderResourceView();
		ComPtr<ID3D11ShaderResourceView> GetWaterRefractionShaderResourceView();

		std::shared_ptr<Simple::Camera> GetCurrentCamera();
		std::shared_ptr<Simple::Camera> GetDefaultCamera();
		std::shared_ptr<const Simple::Texture> GetTexture(const char* aFilePath);
		std::shared_ptr<const Simple::Texture> GetDefaultTexture();
		std::shared_ptr<const Simple::Shader> GetDefaultShader();
		std::shared_ptr<const Simple::Shader> GetShader(const char* aPSFile, const char* aVSFile);

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
		std::unordered_map<std::string, const std::shared_ptr<const Simple::Texture>> myLoadedTextures;
		std::unordered_map<std::pair<std::string, std::string>, std::shared_ptr<const Simple::Shader>, SimpleUtilities::PairHash, SimpleUtilities::PairEqual> myLoadedShaders;
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

		std::shared_ptr<Simple::Camera> myCurrentCamera;
		std::shared_ptr<Simple::Camera> myDefaultCamera;
		std::shared_ptr<const D3D11_VIEWPORT> myViewPort;

		std::unique_ptr<Simple::ConstantBuffer> myCameraConstantBuffer;
		std::unique_ptr<Simple::ConstantBuffer> myTimeConstantBuffer;
		std::unique_ptr<Simple::ConstantBuffer> myLightConstantBuffer;
		std::unique_ptr<Simple::ConstantBuffer> myBonesConstantBuffer;

		std::unique_ptr<LightBufferData> myLightBufferData;

		std::unique_ptr<RenderTarget> myWaterReflectionRenderTarget;
		std::unique_ptr<RenderTarget> myWaterRefractionRenderTarget;
		std::unique_ptr<RenderTarget> myImGuiImageRenderTarget;

		std::unique_ptr<Simple::ModelFactory> myModelFactory;
		std::unique_ptr<Drawer::Renderer> myRenderer;
		std::unique_ptr<Simple::ImGuiEngine> myImGuiEngine;

		float myClearColor[4];
		float myWaterMoveFactor;
		unsigned int myFPSLevelCap;
		bool myVSync;
	};
}