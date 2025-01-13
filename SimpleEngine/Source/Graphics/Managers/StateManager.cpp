#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Managers/StateManager.hpp"
#include <d3d11.h>
#include <cassert>

namespace Graphics
{
	StateManager::StateManager()
		: myCurrentRasterizerState(eRasterizerState::BackfaceCulling)
	{
	}

	StateManager::~StateManager()
	{
	}

	void StateManager::Init(Microsoft::WRL::ComPtr<ID3D11Device> aDevice)
	{
		 CreateRasterizerStates(aDevice);
		 CreateDepthStencilState(aDevice);
		 CreateBlendStates(aDevice);
		 CreateSamplerState(aDevice);
	}

	void StateManager::CreateRasterizerStates(Microsoft::WRL::ComPtr<ID3D11Device> aDevice)
	{
		HRESULT result = S_OK;

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = aDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::Wireframe)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: Wireframe");

		rasterizerDesc = {};
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = aDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::WireframeNoCulling)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: WireframeNoCulling");

		rasterizerDesc = {};
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = aDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::NoFaceCulling)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: NoFaceCulling");

		rasterizerDesc = {};
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = aDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[static_cast<int>(eRasterizerState::FrontFaceCulling)]);
		assert(SUCCEEDED(result) && "Failed to create RasterizerState: FrontFaceCulling");

		myRasterizerStates[static_cast<int>(eRasterizerState::BackfaceCulling)] = nullptr;
	}

	void StateManager::CreateDepthStencilState(Microsoft::WRL::ComPtr<ID3D11Device> aDevice)
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.StencilEnable = false;

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT result = aDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStencilStates[static_cast<size_t>(eDepthStencilState::Less_Equal)].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create DepthStencilState");

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
		result = aDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStencilStates[static_cast<size_t>(eDepthStencilState::Greater)].GetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create DepthStencilState");
	}

	void StateManager::CreateBlendStates(Microsoft::WRL::ComPtr<ID3D11Device> aDevice)
	{
		HRESULT result = S_OK;

		D3D11_BLEND_DESC blendStateDescription = {};
		blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
		blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = aDevice->CreateBlendState(&blendStateDescription, myBlendStates[static_cast<size_t>(eBlendState::Disabled)].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create blend state");

		D3D11_BLEND_DESC blendStateDescription2 = {};
		blendStateDescription2.RenderTarget[0].BlendEnable = TRUE;
		blendStateDescription2.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription2.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendStateDescription2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription2.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription2.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription2.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		blendStateDescription2.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = aDevice->CreateBlendState(&blendStateDescription2, myBlendStates[static_cast<size_t>(eBlendState::AdditiveBlend)].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create blend state");

		D3D11_BLEND_DESC blendStateDescription3 = {};
		blendStateDescription3.RenderTarget[0].BlendEnable = TRUE;
		blendStateDescription3.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription3.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDescription3.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription3.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription3.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription3.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		blendStateDescription3.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = aDevice->CreateBlendState(&blendStateDescription, myBlendStates[static_cast<size_t>(eBlendState::AlphaBlend)].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(result) && "Failed to create blend state");
	}

	void StateManager::CreateSamplerState(Microsoft::WRL::ComPtr<ID3D11Device> aDevice)
	{
		HRESULT result = S_OK;

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		result = aDevice->CreateSamplerState(&samplerDesc, &mySamplerStates[static_cast<size_t>(eSamplerState::Bilinear_Warp)]);
		assert(SUCCEEDED(result) && "Failed to create SamplerState");

		D3D11_SAMPLER_DESC samplerDesc2 = {};
		samplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc2.MipLODBias = 0.0f;
		samplerDesc2.MaxAnisotropy = 16;
		samplerDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc2.BorderColor[0] = 0;
		samplerDesc2.BorderColor[1] = 0;
		samplerDesc2.BorderColor[2] = 0;
		samplerDesc2.BorderColor[3] = 0;
		samplerDesc2.MinLOD = 0;
		samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;

		result = aDevice->CreateSamplerState(&samplerDesc2, &mySamplerStates[static_cast<size_t>(eSamplerState::Trilinear_Clamp)]);
		assert(SUCCEEDED(result) && "Failed to create SamplerState");
	}

	void StateManager::SetRasterizerState(ComPtr<ID3D11DeviceContext> aContext, const eRasterizerState aRasterizerState)
	{
		myCurrentRasterizerState = aRasterizerState;
		aContext->RSSetState(myRasterizerStates[static_cast<int>(myCurrentRasterizerState)].Get());
	}

	void StateManager::SetDepthStencilState(ComPtr<ID3D11DeviceContext> aContext, const eDepthStencilState aDepthStencilState)
	{
		aContext->OMSetDepthStencilState(myDepthStencilStates[static_cast<size_t>(aDepthStencilState)].Get(), 0);
	}

	void StateManager::SetBlendState(ComPtr<ID3D11DeviceContext> aContext,const eBlendState aBlendState)
	{
		aContext->OMSetBlendState(myBlendStates[static_cast<size_t>(aBlendState)].Get(), nullptr, 0xffffffff);
	}

	void StateManager::SetSamplerState(ComPtr<ID3D11DeviceContext> aContext, const eSamplerState aSamplerState)
	{
		aContext->PSSetSamplers(0, 1, mySamplerStates[static_cast<size_t>(aSamplerState)].GetAddressOf());
	}

	const eRasterizerState StateManager::GetCurrentRasterizerState() const
	{
		return myCurrentRasterizerState;
	}
}
