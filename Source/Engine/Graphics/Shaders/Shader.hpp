#pragma once

class Shader final
{
public:
	Shader();
	~Shader();

	const bool Init(ComPtr<ID3D11Device>& aDevice, const std::string& aPSFileName, const std::string& aVSFileName);
	void SetShader(ID3D11DeviceContext* aContext);
private:
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11InputLayout> myInputLayout;
};
