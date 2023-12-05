#include "stdafx.h"
#include "Engine/Graphics/Shaders/Shader.h"
#include <fstream>

Shader::Shader()
{

}

Shader::~Shader()
{
}

const bool Shader::Init(ComPtr<ID3D11Device>& aDevice, const std::string& aPSFileName, const std::string& aVSFileName)
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

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	result = aDevice->CreateInputLayout(inputLayout.data(), static_cast<int>(inputLayout.size()), vsData.data(), vsData.size(), &myInputLayout);
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
