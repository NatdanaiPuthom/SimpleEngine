#include "stdafx.h"
#include "Engine/Graphics/Texture/Texture.h"
#include "Engine/SimpleUtilities/Utility.h"
#include <External/DDSTextureLoader/DDSTextureLoader11.h>

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::Init(ComPtr<ID3D11Device>& aDevice, const char* aFileName)
{
	const std::string texturePath = std::string(SIMPLE_ASSETS_DIR) + std::string(aFileName);
	const std::wstring texturePathW = SimpleUtilities::ToWString(texturePath);

	const HRESULT result = DirectX::CreateDDSTextureFromFile(aDevice.Get(), texturePathW.c_str(), NULL, &myShaderResourceView);
	if (FAILED(result))
		return false;

	return true;
}

void Texture::Bind(ComPtr<ID3D11DeviceContext>& aContext, const unsigned int aSlot)
{
	aContext->PSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
}
