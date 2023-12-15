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
	SimpleUtilities::Vector3f dir;
	float padding;
};

class GraphicsEngine
{
public:
	GraphicsEngine();
	~GraphicsEngine();

	bool Init(const int aWidth, const int aHeight, HWND& aWindowHandle);
	bool BeginFrame();
	void EndFrame();

	void SetToBackBuffer();
	void SetVSync(const bool aShouldTurnOn);
public:
	ComPtr<ID3D11Device> GetDevice();
	ComPtr<ID3D11DeviceContext> GetContext();
	ComPtr<ID3D11ShaderResourceView> GetShaderResourceView();
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

	bool CreateStuffForImGuiImage(const int aWidth, const int aHeight);
private:
	void Update();
	void LoadSettingsFromJson();
private:
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

	std::shared_ptr<Camera> myCamera;

	std::unique_ptr<ConstantBuffer> myCameraBuffer;
	std::unique_ptr<ConstantBuffer> myTimeBuffer;
	std::unique_ptr<ConstantBuffer> myDirectionLightBuffer;

	float myColor[4];
	bool myVSync;
};