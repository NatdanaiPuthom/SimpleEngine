#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/Global.hpp"
#include <External/DDSTextureLoader/DDSTextureLoader11.h>
#include <d3d11.h>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <External/stb/stb_image.h>

namespace Simple
{
	Texture::Texture()
		: mySlot(0)
	{
	}

	Texture::~Texture()
	{
		myShaderResourceView.Reset();
	}

	void Texture::Bind(ComPtr<ID3D11DeviceContext> aContext, const unsigned int aSlot) const
	{
		aContext->PSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
	}

	void Texture::SetSlot(const unsigned int aSlot)
	{
		mySlot = aSlot;
	}

	unsigned int Texture::GetSlot() const
	{
		return mySlot;
	}

	bool Texture::LoadDDS(const char* aFileName)
	{
		const std::string texturePath = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_TEXTURES) + aFileName;
		const std::wstring texturePathW = SimpleUtilities::ToWString(texturePath);

		const HRESULT result = DirectX::CreateDDSTextureFromFile(Global::GetGraphicsEngine()->GetDevice().Get(), Global::GetGraphicsEngine()->GetContext().Get(), texturePathW.c_str(), NULL, &myShaderResourceView);
		if (FAILED(result))
			return false;

		return true;
	}

	bool Texture::LoadNonDDS(const char* aFileName)
	{
		int width, height, channels;
		const std::string texturePath = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_TEXTURES) + aFileName;
		unsigned char* img = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);

		if (img == nullptr)
			return false;

		auto device = Global::GetGraphicsEngine()->GetDevice();

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

			if (!InitNonDDS(device, imageData.data(), width, height))
				return false;
		}
		else if (channels == 4)
		{
			if (!InitNonDDS(device, img, width, height))
				return false;
		}
		else
		{
			return false;
		}

		return true;
	}

	bool Texture::InitNonDDS(ComPtr<ID3D11Device> aDevice, const unsigned char* aRGBAPixels, const int aWidth, const int aHeight)
	{
		//TO-DO: Fix so it works with mipmaps

		ComPtr<ID3D11Texture2D> texture = nullptr;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = aWidth;
		desc.Height = aHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
}