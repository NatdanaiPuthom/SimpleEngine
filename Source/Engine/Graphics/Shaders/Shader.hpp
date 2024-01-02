#pragma once

//TO-DO: Seperate Vertex and Pixel shader

namespace Simple
{
	class Shader final
	{
	public:
		Shader();
		~Shader();

		const bool Init(ComPtr<ID3D11Device>& aDevice, const std::string& aPSFileName, const std::string& aVSFileName);

		void UseThisShader(ID3D11DeviceContext* aContext) const;
		void UseThisPixelShader(ComPtr<ID3D11DeviceContext> aContext) const;
		void UseThisVertexShader(ComPtr<ID3D11DeviceContext> aContext) const;
	private:
		ComPtr<ID3D11VertexShader> myVertexShader;
		ComPtr<ID3D11PixelShader> myPixelShader;
		ComPtr<ID3D11InputLayout> myInputLayout;
	};
}