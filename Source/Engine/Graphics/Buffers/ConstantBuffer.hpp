#pragma once
class GraphicsEngine;

struct ID3D11Buffer;

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	bool Init(GraphicsEngine* aGraphicsEngine, unsigned int aSize, void* aData);
	void Update(unsigned int aSize, void* aData);
	void Bind(int aSlot);
private:
	GraphicsEngine* myGraphicsEngine;
	ComPtr<ID3D11Buffer> myBuffer;
};
