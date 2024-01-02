#pragma once

struct ID3D11Buffer;

namespace Simple
{
	class ConstantBuffer final
	{
	public:
		ConstantBuffer();
		~ConstantBuffer();

		bool Init(const unsigned int aSize, void* aData);
		void Update(unsigned int aSize, void* aData);
		void Bind(const int aSlot);
	public:
		void SetSlot(const unsigned int aSlot);
		const unsigned int GetSlot() const;
	private:
		ComPtr<ID3D11Buffer> myBuffer;
		unsigned int mySlot;
	};
}