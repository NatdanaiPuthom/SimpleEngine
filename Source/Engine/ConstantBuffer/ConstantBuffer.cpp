#include "stdafx.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer()
	: myGraphicsEngine(nullptr)
{
}

ConstantBuffer::~ConstantBuffer()
{
	myGraphicsEngine = nullptr;
}

bool ConstantBuffer::Init(GraphicsEngine* aGraphicsEngine, unsigned int aSize, void* aData)
{
	myGraphicsEngine = aGraphicsEngine;

	if (aSize == 0 || aData == nullptr)
		return false;

	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.ByteWidth = aSize;

	HRESULT result = myGraphicsEngine->GetDevice()->CreateBuffer(&bufferDescription, nullptr, &myBuffer);
	if (FAILED(result))
		return false;

	myGraphicsEngine->GetContext()->UpdateSubresource(myBuffer.Get(), 0, nullptr, aData, 0, 0);

	return true;
}

void ConstantBuffer::Update(unsigned int aSize, void* aData)
{
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	myGraphicsEngine->GetContext()->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, aData, aSize);
	myGraphicsEngine->GetContext()->Unmap(myBuffer.Get(), 0);
}

void ConstantBuffer::Bind(int aSlot)
{
	myGraphicsEngine->GetContext()->VSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
	myGraphicsEngine->GetContext()->PSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}
