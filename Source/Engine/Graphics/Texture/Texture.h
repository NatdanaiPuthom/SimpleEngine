#pragma once

class Texture
{
public:
	Texture();
	~Texture();

	bool Init(ComPtr<ID3D11Device>& aDevice, const char* aFileName);
	void Bind(ComPtr<ID3D11DeviceContext> aContext, const unsigned int aSlot);

private:
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};
