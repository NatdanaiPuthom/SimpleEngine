#pragma once
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace Graphics
{
	enum class eRasterizerState
	{
		BackfaceCulling,
		NoFaceCulling,
		Wireframe,
		WireframeNoCulling,
		FrontFaceCulling,
		Count
	};

	enum class eBlendState
	{
		Disabled,
		AlphaBlend,
		AdditiveBlend,
		Count
	};

	enum class eRenderTarget
	{
		Backbuffer,
		ImGui,
		PostProcessing,
		Count
	};

	enum class eShaderType
	{
		Unlit_Default,
		Unlit_Animated,
		PBR_Default,
		PBR_Animated,
		SkyBox
	};

	enum class eTextureType
	{
		Default,
		SkyBox_DayCloud,
		SkyBox_NightStar,
		SkyBox_DayGrassland,
		SkyBox_AutumnForest,
	};

	struct RenderTarget final
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};
}