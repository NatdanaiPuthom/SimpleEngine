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

	enum class eDepthStencilState
	{
		Less_Equal,
		Greater,
		Count
	};

	enum class eRenderTargetType
	{
		Backbuffer,
		GBuffer,
		Deferred,
		PostProcessing,
		Count
	};

	enum class eShaderType
	{
		Unlit_Default,
		Unlit_Animated,
		PBR_Default,
		PBR_Animated,
		SkyBox,
		GBuffer,
		Deferred,
		PointLight,
		PostProcessing,
		Copy
	};

	enum class eTextureType
	{
		Default,
		Simple,
		SkyBox_DayCloud,
		SkyBox_NightStar,
		SkyBox_DayGrassland,
		SkyBox_AutumnForest,
		TGA_Skansen
	};

	struct RenderTarget final
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};
}