#pragma once
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

enum class eRasterizerState
{
	BackfaceCulling,
	NoFaceCulling,
	Wireframe,
	WireframeNoCulling,
	FrontFaceCulling,
	Count
};

enum class eRenderTarget
{
	Backbuffer,
	ImGui,
	WaterReflection,
	WaterRefraction,
};

namespace Graphics
{
	enum class eSkyboxType
	{
		DayCloud,
		NightStar
	};
}

struct RenderTarget final
{
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};