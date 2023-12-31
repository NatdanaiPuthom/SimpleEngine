#include "Engine/Precomplier/stdafx.h"

namespace Simple
{
	ConstantBuffer::ConstantBuffer()
		: mySlot(100)
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
	}

	bool ConstantBuffer::Init(const unsigned int aSize, void* aData)
	{
		if (aSize == 0 || aData == nullptr)
			return false;

		D3D11_BUFFER_DESC bufferDescription = {};
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = aSize;

		HRESULT result = SimpleGlobal::GetGraphicsEngine()->GetDevice()->CreateBuffer(&bufferDescription, nullptr, &myBuffer);
		if (FAILED(result))
			return false;

		return true;
	}

	void ConstantBuffer::Update(unsigned int aSize, void* aData)
	{
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		SimpleGlobal::GetGraphicsEngine()->GetContext()->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, aData, aSize);
		SimpleGlobal::GetGraphicsEngine()->GetContext()->Unmap(myBuffer.Get(), 0);
	}

	void ConstantBuffer::Bind(const int aSlot)
	{
		SimpleGlobal::GetGraphicsEngine()->GetContext()->VSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
		SimpleGlobal::GetGraphicsEngine()->GetContext()->PSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
	}

	void ConstantBuffer::SetSlot(const unsigned int aSlot)
	{
		mySlot = aSlot;
	}

	const unsigned int ConstantBuffer::GetSlot() const
	{
		return mySlot;
	}
}