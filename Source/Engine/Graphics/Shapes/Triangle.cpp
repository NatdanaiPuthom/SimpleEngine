#include "stdafx.h"
#include <fstream>

#include "engine.h"
#include "global.h"
#include "Triangle.h"

Triangle::Triangle() = default;
Triangle::~Triangle() = default;

bool Triangle::Init(ID3D11Device* aDevice)
{
	HRESULT result;

	if (!InitBuffers(aDevice, myTriangle, "TrianglePS.cso", "TriangleVS.cso"))
		return false;

	{ //Create Time Buffer
		D3D11_BUFFER_DESC timeBufferDesc = {};
		timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
		timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		result = aDevice->CreateBuffer(&timeBufferDesc, nullptr, &myTimeBuffer);

		if (FAILED(result))
			return false;
	}

	return true;
}

void Triangle::Render(ID3D11DeviceContext* aContext)
{
	TimeBufferType* timeData;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	aContext->Map(myTimeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timeData = (TimeBufferType*)mappedResource.pData;
	timeData->elapsedTime = static_cast<float>(SimplyGlobal::GetTotalTime());
	aContext->Unmap(myTimeBuffer.Get(), 0);
	aContext->VSSetConstantBuffers(0, 1, myTimeBuffer.GetAddressOf());

	aContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	unsigned int stride = sizeof(Vertex2);
	unsigned int offset = 0;

	aContext->IASetInputLayout(myTriangle.inputLayout.Get());
	aContext->IASetVertexBuffers(0, 1, myTriangle.vertexBuffer.GetAddressOf(), &stride, &offset);
	aContext->IASetIndexBuffer(myTriangle.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	aContext->VSSetShader(myTriangle.vertexShader.Get(), nullptr, 0);
	aContext->PSSetShader(myTriangle.pixelShader.Get(), nullptr, 0);
	aContext->DrawIndexed(myTriangle.indicesSize, 0, 0);
}

bool Triangle::InitBuffers(ID3D11Device* aDevice, TriangleData& aTriangleData, const std::string& aPSFileName, const std::string& aVSFileName)
{
	HRESULT result;

	Vertex2 vertices[3] =
	{
		{-0.7f,0.0f,0,1,1,0,0,1},
		{-0.5f,0.5f,0,1,0,1,0,1},
		{-0.3f,0.0f,0,1,0,0,1,1}
	};

	unsigned int indices[3] =
	{
		0, 1, 2,
	};

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(vertices);

	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;

	result = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &aTriangleData.vertexBuffer);

	if (FAILED(result))
		return false;


	aTriangleData.indicesSize = 3;

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * aTriangleData.indicesSize;


	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = indices;

	result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &aTriangleData.indexBuffer);

	if (FAILED(result))
		return false;

	std::string vsData;
	std::string psData;
	std::ifstream vsFile;
	std::ifstream psFile;

	vsFile.open((SHADER_DIR + aVSFileName), std::ios::binary);
	vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	vsFile.close();
	result = aDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &aTriangleData.vertexShader);

	if (FAILED(result))
		return false;

	psFile.open((SHADER_DIR + aPSFileName), std::ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	psFile.close();
	result = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &aTriangleData.pixelShader);

	if (FAILED(result))
		return false;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	result = aDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &aTriangleData.inputLayout);

	if (FAILED(result))
		return false;

	return true;
}
