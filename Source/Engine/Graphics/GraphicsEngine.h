#pragma once

class Camera;

struct FrameBufferData
{
	SimpleUtilities::Matrix4x4f worldToClipMatrix;
};

struct ObjectBufferData
{
	SimpleUtilities::Matrix4x4f modelToWorldMatrix;
};

struct TimeBufferData
{
	float time;
	float padding[3];
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
public:
	ComPtr<ID3D11Device>& GetDevice();
	ComPtr<ID3D11DeviceContext>& GetContext();
	ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView();
	std::shared_ptr<Camera> GetCamera();
private:
	void CreateViewport(const int aWidth, const int aHeight);
	bool CreateSwapChain(HWND& aWindowHandle, const int aWidth, const int aHeight);
	bool CreateBackBuffer();
	bool CreateFrameBuffer();
	bool CreateDepthBuffer(const int aWidth, const int aHeight);
	bool CreateSamplerState();

	bool CreateStuffForImGuiImage(const int aWidth, const int aHeight);
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

	float myColor[4];
};