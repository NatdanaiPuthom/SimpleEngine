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

	enum class eSamplerState
	{
		Bilinear_Warp,
		Trilinear_Clamp,
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
		BloomDownAndUpScale,
		Bloom,
		Count
	};

	enum class eShaderType
	{
		Unlit_Default,
		PBR_Default,
		Animated,
		SkyBox,
		Deferred,
		PointLight,
		PostProcessing,
		GaussianBlur,
		Bloom,
		BloomPixelFilter,
		Copy,
		Count
	};

	enum class eTextureType
	{
		Default,
		Default_Albedo,
		Default_Normal,
		Default_Material,
		Simple,
		DirectionalLight
	};

	enum class eSkyBox
	{
		DayCloud,
		NightStar,
		DayGrassland,
		AutumnForest,
		TGA_Skansen
	};

	enum class eIconType
	{
		FBX,
		Folder,
		CubeMap,
		PNG,
		JPG,
		OBJ,
		MP3,
		Scene,
		Cursor,
		Unknown
	};

	struct RenderTarget final
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};
}