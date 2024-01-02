#pragma once
#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class Camera;
class ConstantBuffer;
class Texture;
class Shader;

namespace Simple
{
	class Renderer;
	class ModelFactory;
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
	SimpleUtilities::Matrix4x4f worldToClipMatrix;
	SimpleUtilities::Vector3f cameraPosition;
	float paddingCameraPos;

	SimpleUtilities::Vector2ui resolution;
	float paddingResolution[2] = { -1.0f };

	float waterHeight;
	float paddingWaterHeight[3];
};

struct alignas(16) ObjectBufferData final
{
	SimpleUtilities::Matrix4x4f modelWorldMatrix;
};

struct alignas(16) TimeBufferData final
{
	float totalTime = 0;
	const float padding[3] = { -1 };
};

struct alignas(16) LightBufferData final
{
	SimpleUtilities::Vector4f skyColor = SimpleUtilities::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	SimpleUtilities::Vector4f groundColor = SimpleUtilities::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	SimpleUtilities::Vector3f directionalLightDirection;
	const float paddingDirectionalLightDirection = -1.0f;

	SimpleUtilities::Vector4f directionalLightColor = SimpleUtilities::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	struct PointLightData
	{
		SimpleUtilities::Vector4f position;
		SimpleUtilities::Vector4f color = SimpleUtilities::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		float range = 0.0f;
		const float padding[3] = { -1.0f };
	} pointLights[8];

	unsigned int numberOfPointlights = 0;
	const float paddingPointlightData[3] = { -1.0f };
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

		const bool Init(const SimpleUtilities::Vector2ui& aWindowSize, HWND& aWindowHandle);

		const bool BeginFrame();
		void EndFrame();

		const bool AddTexture(const char* aFileName, const unsigned int aSlot = 0);
		const bool AddShader(const char* aPSFile, const char* aVSFile);

		bool IsVSyncActive() const;
	public:
		void SetDirectionalLightDirection(const SimpleUtilities::Vector3f& aDirection);
		void SetDirectionalLightColor(const SimpleUtilities::Vector4f& aColor);
		void SetSkyColor(const SimpleUtilities::Vector4f& aColor);
		void SetGroundColor(const SimpleUtilities::Vector4f& aColor);
		void SetVSync(const bool aShouldTurnOn);
		void SetFPSLevelCap(const unsigned int aCapLevel);
		void SetRasterizerState(const eRasterizerState aRasterizerState);
		void SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize, const bool aSetFullScreen);
		void SetRenderTarget(eRenderTarget aRenderTarget);
	public:
		ComPtr<ID3D11Device> GetDevice();
		ComPtr<ID3D11DeviceContext> GetContext();
		ComPtr<ID3D11ShaderResourceView> GetImGuiShaderResourceView();
		ComPtr<ID3D11ShaderResourceView> GetWaterShaderResourceView();
		ComPtr<ID3D11ShaderResourceView> GetWaterRefractionShaderResourceView();

		std::shared_ptr<Camera> GetCamera();
		std::shared_ptr<const Texture> GetTexture(const char* aFilePath);
		std::shared_ptr<const Texture> GetDefaultTexture();
		std::shared_ptr<const Shader> GetDefaultShader();
		std::shared_ptr<const Shader> GetShader(const char* aPSFile, const char* aVSFile);

		SimpleUtilities::Vector4f GetDirectionalLightColor() const;
		SimpleUtilities::Vector3f GetDirectionalLightDirection() const;
		SimpleUtilities::Vector4f GetSkyColor() const;
		SimpleUtilities::Vector4f GetGroundColor() const;

		unsigned int GetFPSLevelCap() const;
	private:
		void CreateViewport(const int aWidth, const int aHeight);
		bool CreateSwapChain(HWND& aWindowHandle, const int aWidth, const int aHeight);
		bool CreateDepthBuffer(const int aWidth, const int aHeight);
		bool CreateDepthStencilState();
		bool CreateBackBuffer();
		bool CreateFrameBuffer();
		bool CreateSamplerState();
		bool CreateCameraBuffer();
		bool CreateTimeBuffer();
		bool CreateLightBuffer();
		bool CreateRenderTargetForImGuiImage(const int aWidth, const int aHeight);
		bool CreateWaterRenderTarget(const int aWidth, const int aHeight);
		bool CreateWaterRefractionRenderTarget(const int aWidth, const int aHeight);
		bool CreateRasterizerStates();
	private:
		void Update();
		void LoadSettingsFromJson();
		void LoadTextures();
		void LoadShaders();
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

		std::shared_ptr<Camera> myCamera;
		std::shared_ptr<const D3D11_VIEWPORT> myViewPort;

		std::unique_ptr<ConstantBuffer> myCameraConstantBuffer;
		std::unique_ptr<ConstantBuffer> myTimeConstantBuffer;
		std::unique_ptr<ConstantBuffer> myLightBuffer;

		std::unique_ptr<LightBufferData> myLightBufferData;

		std::unique_ptr<RenderTarget> myWaterReflectionRenderTarget;
		std::unique_ptr<RenderTarget> myWaterRefractionRenderTarget;
		std::unique_ptr<RenderTarget> myImGuiImageRenderTarget;

		std::unique_ptr<Simple::Renderer> myRenderer;
		std::unique_ptr<Simple::ModelFactory> myModelFactory;

		float myClearColor[4];
		unsigned int myFPSLevelCap;
		bool myVSync;
	};
}