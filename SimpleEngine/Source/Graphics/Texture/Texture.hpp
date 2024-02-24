#pragma once
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;
struct ID3D11DeviceContext;
struct ID3D11Device;

namespace Simple
{
	class Texture final
	{
	public:
		Texture();
		~Texture();

		void Bind(ComPtr<ID3D11DeviceContext> aContext, const unsigned int aSlot) const;
		bool LoadDDS(const char* aFileName);
		bool LoadNonDDS(const char* aFileName);
	public:
		void SetSlot(const unsigned int aSlot);
		unsigned int GetSlot() const;
	private:
		bool InitNonDDS(ComPtr<ID3D11Device> aDevice, const unsigned char* aRGBAPixels, const int aWidth, const int aHeight);
	private:
		ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
		unsigned int mySlot;
	};
}