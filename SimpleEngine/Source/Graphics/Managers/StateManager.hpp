#pragma once
#include "Graphics/GraphicsDeclarations.hpp"
#include <array>

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11SamplerState;
struct ID3D11BlendState;
struct ID3D11DeviceContext;
struct ID3D11Device;

namespace Graphics
{
	class StateManager final
	{
	public:
		StateManager();
		~StateManager();

		void Init(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	public:
		void SetRasterizerState(ComPtr<ID3D11DeviceContext> aContext, const eRasterizerState aRasterizerState);
		void SetDepthStencilState(ComPtr<ID3D11DeviceContext> aContext, const eDepthStencilState aDepthStencilState);
		void SetBlendState(ComPtr<ID3D11DeviceContext> aContext, const eBlendState aBlendState);
		void SetSamplerState(ComPtr<ID3D11DeviceContext> aContext, const eSamplerState aSamplerState);
	public:
		const eRasterizerState GetCurrentRasterizerState() const;
	private:
		void CreateRasterizerStates(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
		void CreateDepthStencilState(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
		void CreateBlendStates(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
		void CreateSamplerState(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	private:
		std::array<Microsoft::WRL::ComPtr<ID3D11RasterizerState>, static_cast<size_t>(eRasterizerState::Count)> myRasterizerStates;
		std::array<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>, static_cast<size_t>(eDepthStencilState::Count)> myDepthStencilStates;
		std::array<Microsoft::WRL::ComPtr<ID3D11BlendState>, static_cast<size_t>(eBlendState::Count)> myBlendStates;
		std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, static_cast<size_t>(eSamplerState::Count)> mySamplerStates;

		eRasterizerState myCurrentRasterizerState;
	};
}