#pragma once

class Texture
{
public:
	Texture();
	~Texture();

	bool Init(ComPtr<ID3D11Device> aDevice, const char* aFileName);
	void Bind(ComPtr<ID3D11DeviceContext> aContext, const unsigned int aSlot);

	bool Initialize(ComPtr<ID3D11Device> aDevice, unsigned char* aRGBAPixels, int aWidth, int aHeight); //Test
	bool Test(ComPtr<ID3D11Device> aDevice); //Test
private:
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};
