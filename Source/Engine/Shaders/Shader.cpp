#include "stdafx.h"
#include <fstream>
#include "Shaders/Shader.h"

Shader::Shader()
{

}

Shader::~Shader()
{
}

bool Shader::Init(ID3D11Device* aDevice, std::vector<D3D11_INPUT_ELEMENT_DESC> aInputLayout, const std::string& aPSFileName, const std::string& aVSFileName)
{
	HRESULT result;
	std::string vsData;
	std::string psData;
	std::ifstream vsFile;
	std::ifstream psFile;

	vsFile.open((SHADER_DIR + aVSFileName), std::ios::binary);
	vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	vsFile.close();

	result = aDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
	if (FAILED(result))
		return false;

	psFile.open((SHADER_DIR + aPSFileName), std::ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	psFile.close();

	result = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
	if (FAILED(result))
		return false;

	result = aDevice->CreateInputLayout(aInputLayout.data(), static_cast<int>(aInputLayout.size()), vsData.data(), vsData.size(), &myInputLayout);
	if (FAILED(result))
		return false;

	return true;
}

void Shader::SetShader(ID3D11DeviceContext* aContext)
{
	aContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
	aContext->PSSetShader(myPixelShader.Get(), nullptr, 0);
	aContext->IASetInputLayout(myInputLayout.Get());
}
