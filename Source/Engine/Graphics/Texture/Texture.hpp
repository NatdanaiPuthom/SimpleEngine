#pragma once

class Texture final
{
public:
	Texture();
	~Texture();

	void Bind(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext, const unsigned int aSlot);
	bool LoadDDS(const char* aFilePath);
	bool LoadNonDDS(const char* aFilePath);
public:
	void SetSlot(const unsigned int aSlot);
	unsigned int GetSlot() const;
private:
	bool InitNonDDS(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const unsigned char* aRGBAPixels, const int aWidth, const int aHeight);
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
	unsigned int mySlot;
};
