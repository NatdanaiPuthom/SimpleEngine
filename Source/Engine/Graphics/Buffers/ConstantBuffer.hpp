#pragma once

class GraphicsEngine;

struct ID3D11Buffer;

class ConstantBuffer final
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	bool Init(GraphicsEngine* aGraphicsEngine, unsigned int aSize, void* aData);
	void Update(unsigned int aSize, void* aData);
	void Bind(const int aSlot);
public:
	void SetSlot(const unsigned int aSlot);
	const unsigned int GetSlot() const;
private:
	GraphicsEngine* myGraphicsEngine;
	ComPtr<ID3D11Buffer> myBuffer;
	unsigned int mySlot;
};
