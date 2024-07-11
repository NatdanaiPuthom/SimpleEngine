#pragma once
#include <wrl/client.h>
#include <string>
#include <d3d11.h> //TO-DO(v11.1.1): why do I have to include this when I already forward declare structs?

using Microsoft::WRL::ComPtr;

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11DeviceContext;
struct ID3D11Device;

namespace Graphics
{
	class Shader final
	{
	public:
		Shader();
		~Shader();

		const bool Init(ID3D11Device* aDevice, const std::string& aPSFilePath, const std::string& aVSFilePath);

		void BindThisShader(ID3D11DeviceContext* aContext) const;
		void BindOnlyThisPixelShader(ID3D11DeviceContext* aContext) const;
		void BindOnlyThisVertexShader(ID3D11DeviceContext* aContext) const;

		const std::string& GetPixelShaderName() const;
		const std::string& GetVertexShaderName() const;

		const std::string& GetVertexShaderRelativePath() const;
		const std::string& GetPixelShaderRelativePath() const;

		ComPtr<ID3D11VertexShader> GetVertexShader() const { return myVertexShader; };
		ComPtr<ID3D11PixelShader> GetPixelShader() const { return myPixelShader; };

	private:
		std::string myPixelShaderName;
		std::string myVertexShaderName;

		std::string myVertexShaderRelativePath;
		std::string myPixelShaderRelativePath;

		ComPtr<ID3D11VertexShader> myVertexShader;
		ComPtr<ID3D11PixelShader> myPixelShader;
		ComPtr<ID3D11InputLayout> myInputLayout;

		const char myPadding[8] = "Believe";
	};
}