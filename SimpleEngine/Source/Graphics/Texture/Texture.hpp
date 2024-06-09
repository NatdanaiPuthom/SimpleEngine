#pragma once
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;
struct ID3D11DeviceContext;
struct ID3D11Device;

namespace Graphics
{
	class Texture final
	{
	public:
		Texture();
		~Texture();

		void Bind(ComPtr<ID3D11DeviceContext> aContext, const unsigned int aSlot) const;
		void Bind(ComPtr<ID3D11DeviceContext> aContext) const;

		bool LoadDDS(const char* aFileName);
		bool LoadNonDDS(const char* aFileName);

	public:
		void SetSlot(const unsigned int aSlot);

		unsigned int GetSlot() const;
		std::string GetShaderName() const;
		const ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const;

	private:
		bool InitNonDDS(ComPtr<ID3D11Device> aDevice, const unsigned char* aRGBAPixels, const int aWidth, const int aHeight);
	private:
		ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
		std::string myName;
		unsigned int mySlot;
	};
}