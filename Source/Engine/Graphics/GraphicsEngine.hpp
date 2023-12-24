#pragma once

class Camera;
class ConstantBuffer;
class Texture;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;

struct alignas(16) FrameBufferData final
{
	SimpleUtilities::Matrix4x4f worldToClipMatrix;
	SimpleUtilities::Vector3f cameraPosition;
	SimpleUtilities::Vector2ui resolution;
	float padding[3] = { -1.0f };
};

struct alignas(16) ObjectBufferData final
{
	SimpleUtilities::Matrix4x4f modelToWorldMatrix;
};

struct alignas(16) TimeBufferData final
{
	float time = 0;
	const float padding[3] = { -1 };
};

struct alignas(16) DirectionalLightBufferData final
{
	SimpleUtilities::Vector3f direction;
	const float padding1 = -1.0f;

	SimpleUtilities::Vector4f color;
};

struct alignas(16) AmbientLightBufferData final
{
	SimpleUtilities::Vector3f skyColor;
	const float padding1 = -1.0f;

	SimpleUtilities::Vector3f groundColor;
	const float padding2 = -1.0f;
};

struct RenderTarget final
{
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};

class GraphicsEngine final
{
public:
	GraphicsEngine();
	~GraphicsEngine();

	bool Init(const SimpleUtilities::Vector2ui& aWindowSize, HWND& aWindowHandle);
	bool BeginFrame();
	void EndFrame();

	bool AddTexture(const char* aFilePath, const unsigned int aSlot = 0);
public:
	void SetDirectionalLightDirection(const SimpleUtilities::Vector3f& aDirection);
	void SetDirectionalLightColor(const SimpleUtilities::Vector4f& aColor);
	void SetSkyColor(const SimpleUtilities::Vector3f& aColor);
	void SetGroundColor(const SimpleUtilities::Vector3f& aColor);
	void SetVSync(const bool aShouldTurnOn);
	void SetToBackBuffer();
	void SetToImGuiBuffer();
	void SetToWaterReflectionBuffer();
public:
	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetImGuiShaderResourceView();

	std::shared_ptr<Camera> GetCamera();

	Texture* GetTexture(const char* aFilePath);

	SimpleUtilities::Vector3f GetDirectionalLightDirection() const;
	SimpleUtilities::Vector4f GetDirectionalLightColor() const;
	SimpleUtilities::Vector3f GetSkyColor() const;
	SimpleUtilities::Vector3f GetGroundColor() const;
	bool IsVSyncActive() const;
private:
	void CreateViewport(const int aWidth, const int aHeight);
	bool CreateSwapChain(HWND& aWindowHandle, const int aWidth, const int aHeight);
	bool CreateDepthBuffer(const int aWidth, const int aHeight);
	bool CreateBackBuffer();
	bool CreateFrameBuffer();
	bool CreateSamplerState();
	bool CreateCameraBuffer();
	bool CreateTimeBuffer();
	bool CreateDirectionalLightBuffer();
	bool CreateAmbientLightBuffer();
	bool CreateRenderTargetForImGuiImage(const int aWidth, const int aHeight);
	bool CreateWaterRenderTarget(const int aWidth, const int aHeight);
	bool CreateFrontFaceCullingRasterizerState();
private:
	void Update();
	void LoadSettingsFromJson();
	void LoadTextures();
private:
	std::shared_ptr<Camera> myCamera;

	Microsoft::WRL::ComPtr<ID3D11Device> myDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> myContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mySwapChain;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> myBackBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDepthBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> myDepthStencilState;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myFrameBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myObjectBuffer;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> mySamplerState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> myFrontFaceCullingRasterizerState;

	std::unique_ptr<ConstantBuffer> myCameraBuffer;
	std::unique_ptr<ConstantBuffer> myTimeBuffer;
	std::unique_ptr<ConstantBuffer> myDirectionLightBuffer;
	std::unique_ptr<ConstantBuffer> myAmbientLightBuffer;

	std::unique_ptr<DirectionalLightBufferData> myDirectionLightData;
	std::unique_ptr<AmbientLightBufferData> myAmbientLightData;

	std::unique_ptr<RenderTarget> myWaterReflectionRenderTarget;
	std::unique_ptr<RenderTarget> myImGuiImageRenderTarget;

	std::unordered_map<std::string, std::unique_ptr<Texture>> myLoadedTextures;

	float myColor[4];
	bool myVSync;
};
