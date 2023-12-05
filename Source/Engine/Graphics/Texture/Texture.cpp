#include "stdafx.h"
#include "Texture.h"

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::Init(ComPtr<ID3D11Device>& aDevice, unsigned char* aRGBAPixels, const int aWidth, const int aHeight)
{
	ComPtr<ID3D11Texture2D> texture = {};

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
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

void Texture::Bind(ComPtr<ID3D11DeviceContext>& aContext, const int aSlot)
{
	aContext->PSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
}
