#pragma once
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11DeviceContext;
struct ID3D11Device;

namespace Simple
{
	class Shader final
	{
	public:
		Shader();
		~Shader();

		const bool Init(ComPtr<ID3D11Device>& aDevice, const std::string& aPSFileName, const std::string& aVSFileName);

		void BindThisShader(ID3D11DeviceContext* aContext) const;
		void BindOnlyThisPixelShader(ComPtr<ID3D11DeviceContext> aContext) const;
		void BindOnlyThisVertexShader(ComPtr<ID3D11DeviceContext> aContext) const;
	private:
		ComPtr<ID3D11VertexShader> myVertexShader;
		ComPtr<ID3D11PixelShader> myPixelShader;
		ComPtr<ID3D11InputLayout> myInputLayout;
	};
}