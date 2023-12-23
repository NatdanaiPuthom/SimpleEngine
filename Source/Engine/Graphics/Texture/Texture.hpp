#pragma once

class Texture final
{
public:
	Texture();
	~Texture();

	bool LoadDDS(const char* aFilePath);
	bool LoadNonDDS(const char* aFilePath);
	void Bind(ComPtr<ID3D11DeviceContext> aContext, const unsigned int aSlot);

private:
	bool InitNonDDS(ComPtr<ID3D11Device> aDevice, const unsigned char* aRGBAPixels, const int aWidth, const int aHeight);
private:
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};
