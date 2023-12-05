#pragma once

class Texture
{
public:
	Texture();
	~Texture();

	bool Init(ComPtr<ID3D11Device>& aDevice, unsigned char* aRGBAPixels, const int aWidth, const int aHeight);
	void Bind(ComPtr<ID3D11DeviceContext>& aContext, const int aSlot);

private:
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};
