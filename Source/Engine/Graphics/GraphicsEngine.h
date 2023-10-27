#pragma once

class Camera;
class Triangle;
class Model;
class Shader;

#include "Camera/Cube.h"

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
};

class GraphicsEngine
{
public:
	GraphicsEngine();
	~GraphicsEngine();

	bool Init(const int aHeight, const int aWidth, HWND& aWindowHandle);
	bool BeginFrame();
	void EndFrame();

	void Render();
	void Draw(Model& aModel, Shader& aShader);
	void Test();
public:
	ComPtr<ID3D11Device>& GetDevice();
	ComPtr<ID3D11DeviceContext>& GetContext();
	std::shared_ptr<Camera> GetCamera();
private:
	void CreateViewport(const int aHeight, const int aWidth);
	bool CreateSwapChain(HWND& aWindowHandle, const int aHeight, const int aWidth);
	bool CreateBackBuffer();
	bool CreateFrameBuffer();
	bool CreateDepthBuffer(const int aHeight, const int aWidth);
private:
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;

	ComPtr<ID3D11RenderTargetView> myBackBuffer;
	ComPtr<ID3D11DepthStencilView> myDepthBuffer;
	ComPtr<ID3D11DepthStencilState> myDepthStencilState;

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;
	
	std::shared_ptr<Camera> myCamera;
	std::shared_ptr<Triangle> myTriangle;

	Cube myCube;

	float myColor[4];
};