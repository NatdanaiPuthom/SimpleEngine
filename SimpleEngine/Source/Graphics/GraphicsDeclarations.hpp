#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#define CAMERA_CONSTANT_BUFFER_SLOT 0
#define OBJECT_CONSTANT_BUFFER_SLOT 1
#define TIME_CONSTANT_BUFFER_SLOT 2
#define LIGHT_CONSTANT_BUFFER_SLOT 3
#define BONES_CONSTANT_BUFFER_SLOT 4

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

	enum class eRenderTarget
	{
		Backbuffer,
		ImGui,
		PostProcessing,
		Count
	};

	enum class eSkyboxType
	{
		DayCloud,
		NightStar,
		DayGrassland,
		AutumnForest
	};

	struct RenderTarget final
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};
}