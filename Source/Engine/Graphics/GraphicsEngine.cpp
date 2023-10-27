#include "stdafx.h"
#include "Triangle/Triangle.h"
#include "Camera/Camera.h"

//#define REPORT_DX_WARNINGS

GraphicsEngine::GraphicsEngine()
	: myTriangle(std::make_shared<Triangle>())
	, myCamera(std::make_shared<Camera>())
	, myColor{ 0.0f, 0.25f, 0.50f, 1.0f }
{
}

GraphicsEngine::~GraphicsEngine()
{
}

bool GraphicsEngine::Init(const int aHeight, const int aWidth, HWND& aWindowHandle)
{
#if defined (REPORT_DX_WARNINGS)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	if (!CreateSwapChain(aWindowHandle, aHeight, aWidth))
		return false;

	if (!CreateDepthBuffer(aHeight, aWidth))
		return false;

	if (!CreateBackBuffer())
		return false;

	CreateViewport(aHeight, aWidth);

	if (!CreateFrameBuffer())
		return false;

	if (!myTriangle->Init(myDevice.Get()))
		return false;

	if (myCube.Create())
		std::cout << "Succeess" << std::endl;
	else
		std::cout << "Failed" << std::endl;

	return true;
}

bool GraphicsEngine::BeginFrame()
{
	MSG msg = { 0 };

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			return false;
	}

	Render();
	myCube.Draw();
	return true;
}

void GraphicsEngine::EndFrame()
{
	mySwapChain->Present(1, 0);
}

void GraphicsEngine::Render()
{
	myContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());

	myContext->ClearRenderTargetView(myBackBuffer.Get(), myColor);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	myTriangle->Render(myContext.Get());
}

void GraphicsEngine::Draw(Model& aModel, Shader& aShader)
{
	ComPtr<ID3D11Buffer> vertexBuffer = aModel.GetVertexBuffer();
	ComPtr<ID3D11Buffer> indexBuffer = aModel.GetIndexBuffer();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	myContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	myContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	aShader.SetShader(myContext.Get());

	ObjectBufferData objectBufferData;
	objectBufferData.modelToWorldMatrix = aModel.GetModelToWorldMatrix();

	myContext->DrawIndexed(aModel.GetIndexCount(), 0, 0);
}

void GraphicsEngine::Test()
{
	{
		FrameBufferData frameBufferData = {};
		frameBufferData.worldToClipMatrix = SimpleUtilities::Matrix4x4f::GetFastInverse(myCamera->GetModelToWorldMatrix()) * myCamera->GetProjectionMatrix();

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		myContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &frameBufferData, sizeof(FrameBufferData));
		myContext->Unmap(myFrameBuffer.Get(), 0);
		myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
		myContext->UpdateSubresource(myFrameBuffer.Get(), 0, nullptr, &frameBufferData, 0, 0);
	}

	{
		ObjectBufferData objectBufferData = {};
		objectBufferData.modelToWorldMatrix = SimpleUtilities::Matrix4x4f::GetFastInverse(myCamera->GetModelToWorldMatrix()) * myCamera->GetProjectionMatrix();
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		myContext->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &objectBufferData, sizeof(ObjectBufferData));
		myContext->Unmap(myObjectBuffer.Get(), 0);
		myContext->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
		myContext->UpdateSubresource(myFrameBuffer.Get(), 0, nullptr, &objectBufferData, 0, 0);
	}

}

ComPtr<ID3D11Device>& GraphicsEngine::GetDevice()
{
	return myDevice;
}

ComPtr<ID3D11DeviceContext>& GraphicsEngine::GetContext()
{
	return myContext;
}

std::shared_ptr<Camera> GraphicsEngine::GetCamera()
{
	return myCamera;
}

void GraphicsEngine::CreateViewport(const int aHeight, const int aWidth)
{
	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float> (aWidth);
	viewport.Height = static_cast<float> (aHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	myContext->RSSetViewports(1, &viewport);
}

bool GraphicsEngine::CreateSwapChain(HWND& aWindowHandle, const int aHeight, const int aWidth)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = aWidth;
	swapChainDesc.BufferDesc.Height = aHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	UINT creationFlags = 0;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myContext
	);

	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::CreateBackBuffer()
{
	ID3D11Texture2D* backBufferTexture = nullptr;

	HRESULT result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myBackBuffer);
	backBufferTexture->Release();

	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::CreateFrameBuffer()
{
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.ByteWidth = sizeof(FrameBufferData);

	HRESULT result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	if (FAILED(result))
		return false;

	bufferDescription.ByteWidth = sizeof(ObjectBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::CreateDepthBuffer(const int aHeight, const int aWidth)
{
	ComPtr<ID3D11Texture2D> depthBufferTexture;
	D3D11_TEXTURE2D_DESC depthBufferDesc = { 0 };
	depthBufferDesc.Height = aHeight;
	depthBufferDesc.Width = aWidth;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	HRESULT result = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthBufferTexture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateDepthStencilView(depthBufferTexture.Get(), nullptr, &myDepthBuffer);
	if (FAILED(result))
		return false;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	result = myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStencilState.GetAddressOf());
	if (FAILED(result))
		return false;

	return true;
}
