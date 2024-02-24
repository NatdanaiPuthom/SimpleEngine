#pragma once
#include <wrl/client.h>

struct ID3D11Buffer;

namespace Simple
{
	class ConstantBuffer final
	{
	public:
		ConstantBuffer();
		~ConstantBuffer();

		bool Init(const unsigned int aSize, void* aData);
		void Update(const unsigned int aSize, void* aData);
		void Bind(const int aSlot);
	public:
		void SetSlot(const unsigned int aSlot);
		const unsigned int GetSlot() const;
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> myBuffer;
		unsigned int mySlot;
	};
}