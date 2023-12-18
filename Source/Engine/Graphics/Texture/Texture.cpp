#include "stdafx.h"
#include "Engine/Graphics/Texture/Texture.hpp"
#include <External/DDSTextureLoader/DDSTextureLoader11.h>

#define STB_IMAGE_IMPLEMENTATION
#include <External/stb/stb_image.h>

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::Init(ComPtr<ID3D11Device> aDevice, const char* aFileName)
{
	const std::string texturePath = SimpleUtilities::GetPath(aFileName);
	const std::wstring texturePathW = SimpleUtilities::ToWString(texturePath);

	const HRESULT result = DirectX::CreateDDSTextureFromFile(aDevice.Get(), texturePathW.c_str(), NULL, &myShaderResourceView);
	if (FAILED(result))
		return false;

	return true;
}

void Texture::Bind(ComPtr<ID3D11DeviceContext> aContext, const unsigned int aSlot)
{
	aContext->PSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
}

bool Texture::Initialize(ComPtr<ID3D11Device> aDevice, unsigned char* aRGBAPixels, int aWidth, int aHeight)
{
	ComPtr<ID3D11Texture2D> texture = nullptr;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresourceDesc = {};

	subresourceDesc.pSysMem = (void*)aRGBAPixels;
	subresourceDesc.SysMemPitch = aWidth * 4;
	subresourceDesc.SysMemSlicePitch = aWidth * aHeight * 4;

	if (FAILED(aDevice->CreateTexture2D(&desc, &subresourceDesc, &texture)))
		return false;

	HRESULT result = aDevice->CreateShaderResourceView(texture.Get(), NULL, &myShaderResourceView);
	if (FAILED(result))
		return false;

	return true;
}

bool Texture::Test(ComPtr<ID3D11Device> aDevice)
{
	int width, height, channels;
	auto fileName = SimpleUtilities::GetPath("Assets/fasterthanlight.jpg");
	unsigned char* img = stbi_load(fileName.c_str(), &width, &height, &channels, 0);

	if (img == nullptr)
		return false;

	if (channels == 3)
	{
		std::vector<unsigned char> imageData(width * height * 4);

		for (int i = 0; i < width * height; ++i)
		{
			imageData[static_cast<size_t>(4 * i + 0)] = img[3 * i + 0];
			imageData[static_cast<size_t>(4 * i + 1)] = img[3 * i + 1];
			imageData[static_cast<size_t>(4 * i + 2)] = img[3 * i + 2];
			imageData[static_cast<size_t>(4 * i + 3)] = 255;
		}

		if (!Initialize(aDevice, imageData.data(), width, height))
			return false;
	}
	else if (channels == 4)
	{
		if (!Initialize(aDevice, img, width, height))
			return false;
	}
	else
	{
		return false;
	}

	return true;
}
