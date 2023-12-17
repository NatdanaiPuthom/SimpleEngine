#pragma once

class Camera;
class ConstantBuffer;

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

struct alignas(16) FrameBufferData
{
	SimpleUtilities::Matrix4x4f worldToClipMatrix;
};

struct alignas(16) ObjectBufferData
{
	SimpleUtilities::Matrix4x4f modelToWorldMatrix;
};

struct alignas(16) TimeBufferData
{
	float time;
	float padding[3];
};

struct alignas(16) DirectionalLightBufferData
{
	SimpleUtilities::Vector3f direction;
	float padding1;

	SimpleUtilities::Vector3f color;
	float padding2;
};

struct alignas(16) AmbientLightBufferData
{
	SimpleUtilities::Vector3f skyColor;
	float padding1;

	SimpleUtilities::Vector3f groundColor;
	float padding2;
};

class GraphicsEngine
{
public:
	GraphicsEngine();
	~GraphicsEngine();

	bool Init(const int aWidth, const int aHeight, HWND& aWindowHandle);
	bool BeginFrame();
	void EndFrame();

public:
	void SetDirectionalLightDirection(const SimpleUtilities::Vector3f& aDirection);
	void SetDirectionalLightColor(const SimpleUtilities::Vector3f& aColor);
	void SetSkyColor(const SimpleUtilities::Vector3f& aColor);
	void SetGroundColor(const SimpleUtilities::Vector3f& aColor);
	void SetToBackBuffer();
	void SetVSync(const bool aShouldTurnOn);
public:
	ComPtr<ID3D11Device> GetDevice();
	ComPtr<ID3D11DeviceContext> GetContext();
	ComPtr<ID3D11ShaderResourceView> GetShaderResourceView();

	SimpleUtilities::Vector3f GetDirectionalLightDirection() const;
	SimpleUtilities::Vector3f GetDirectionalLightColor() const;
	SimpleUtilities::Vector3f GetSkyColor() const;
	SimpleUtilities::Vector3f GetGroundColor() const;
	std::shared_ptr<Camera> GetCamera();
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
	bool CreateStuffForImGuiImage(const int aWidth, const int aHeight);
private:
	void Update();
	void LoadSettingsFromJson();
private:
	std::shared_ptr<Camera> myCamera;

	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;

	ComPtr<ID3D11RenderTargetView> myBackBuffer;
	ComPtr<ID3D11DepthStencilView> myDepthBuffer;
	ComPtr<ID3D11DepthStencilState> myDepthStencilState;

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;

	ComPtr<ID3D11SamplerState> mySamplerState;

	ComPtr<ID3D11Texture2D> myTexture;
	ComPtr<ID3D11ShaderResourceView> mySRV;
	ComPtr<ID3D11RenderTargetView> myRTV;

	std::unique_ptr<ConstantBuffer> myCameraBuffer;
	std::unique_ptr<ConstantBuffer> myTimeBuffer;
	std::unique_ptr<ConstantBuffer> myDirectionLightBuffer;
	std::unique_ptr<ConstantBuffer> myAmbientLightBuffer;

	std::unique_ptr<DirectionalLightBufferData> myDirectionLightData;
	std::unique_ptr<AmbientLightBufferData> myAmbientLightData;

	float myColor[4];
	bool myVSync;
};