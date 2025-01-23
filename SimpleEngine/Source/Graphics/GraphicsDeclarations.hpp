#pragma once
#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include <tuple>
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace Graphics
{
	using MeshTextureKey = std::tuple<const Graphics::Mesh*, const Graphics::Texture*, const Graphics::Texture*, const Graphics::Texture*>;

	constexpr size_t GLOBAL_INSTANCER_MAX_INSTANCE = 1024;

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
		Instanced_Unlit_Default,
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

	struct RenderTarget final
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};

	struct MeshTextureKeyHash
	{
		std::size_t operator()(const MeshTextureKey& key) const
		{
			return
				std::hash<const Graphics::Mesh*>()(std::get<0>(key)) ^
				std::hash<const Graphics::Texture*>()(std::get<1>(key)) ^
				std::hash<const Graphics::Texture*>()(std::get<2>(key)) ^
				std::hash<const Graphics::Texture*>()(std::get<3>(key));
		}
	};

	struct MeshInstance
	{
		MeshInstance(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent)
			: transform(&aTransformComponent->transform)
			, mesh(aMeshComponent->mesh)
			, albedoTexture(aMeshComponent->textures[Graphics::Global_Slot_Albedo])
			, normalTexture(aMeshComponent->textures[Graphics::Global_Slot_Normal])
			, materialTexture(aMeshComponent->textures[Graphics::Global_Slot_Material])
		{
		}

		const Math::Transform* const transform;
		const Graphics::Mesh* mesh = nullptr;
		const Graphics::Texture* albedoTexture = nullptr;
		const Graphics::Texture* normalTexture = nullptr;
		const Graphics::Texture* materialTexture = nullptr;
	};
}