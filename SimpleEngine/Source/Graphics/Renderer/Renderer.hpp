#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/SimpleUtilities/Bounds.hpp"
#include "Graphics/Renderer/Drawer/BoundingBoxDrawer.hpp"
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/Renderer/Drawer/SphereDrawer.hpp"
#include "Graphics/Renderer/Drawer/SpriteDrawer.hpp"
#include "Graphics/BufferData.hpp"
#include <memory>
#include <vector>
#include <wrl/client.h>

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace ECS
{
	struct MeshComponent;
	struct TransformComponent;
	struct AnimationComponent;
}

namespace Graphics
{
	class ConstantBuffer;
	class Mesh;
	class Shader;
	class Texture;
}

namespace Simple
{
	struct BoundingBox3D;
}




#include <unordered_map>
#include <tuple>

using MeshTextureKey = std::tuple<const Graphics::Mesh*, const Graphics::Texture*, const Graphics::Texture*, const Graphics::Texture*>;

struct TestMeshInstance
{
	Math::Transform transform;
	const Graphics::Mesh* mesh;
	const Graphics::Texture* albedoTexture;
	const Graphics::Texture* normalTexture;
	const Graphics::Texture* materialTexture;
};

struct MeshTextureKeyHash
{
	std::size_t operator()(const MeshTextureKey& key) const
	{
		return std::hash<const Graphics::Mesh*>()(std::get<0>(key)) ^
			std::hash<const Graphics::Texture*>()(std::get<1>(key)) ^
			std::hash<const Graphics::Texture*>()(std::get<2>(key)) ^
			std::hash<const Graphics::Texture*>()(std::get<3>(key));
	}
};

namespace Drawer
{
	struct BoundingBox3DData
	{
		const Math::Matrix4x4f modelToWorld;
		const Simple::BoundingBox3D boundingBox;
		Simple::Color color;
	};

	class Renderer final
	{
	public:
		void RenderSortedInstances();
		void GenerateInstanceData(std::vector<TransformBufferData>& instanceData);
		void UpdateInstanceBuffer(const std::vector<TransformBufferData>& instanceData);
		void RenderInstances();

		Renderer();
		~Renderer();

		void Init();

		void RenderPBRStaticModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent) const;
		void RenderPBRAnimatedModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent, const ECS::AnimationComponent* aAnimationPlayerComponent) const;

		void RenderUnlitStaticModel(const Math::Matrix4x4f& aTransformMatrix, const Graphics::Mesh* aMesh, const Graphics::Shader* aShader, const Graphics::Texture* aTexture) const;
		void RenderUnlitStaticAnimatedModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent, const ECS::AnimationComponent* aAnimationPlayerComponent) const;

		void RenderSprite2D(const Drawer::Sprite2D& aSprite);

		void Push(const Drawer::Line& aLine);
		void Push(const Drawer::Sphere& aSphere);
		void Push(const Drawer::BoundingBox3DData& aBoundingBoxData);
		void Push(const std::vector<Drawer::Line>& aLines);
		void RenderStaticSkeletonLines(const ECS::TransformComponent* aTransformComponent, const ECS::AnimationComponent* aAnimationPlayerComponent);
		void RenderAnimatedSkeletonLines(const ECS::TransformComponent* aTransformComponent, const ECS::AnimationComponent* aAnimationPlayerComponent);
		void RenderDebugLines();
	public:
		void SetShouldRenderMesh(const bool aShouldRender);
		void SetShouldRenderDebugLines(const bool aShouldRender);
		void SetShouldRenderBoundingBox(const bool aShouldRender);
		void SetShouldRenderSkeletonLines(const bool aShouldRender);
		void SetIsUsingPBR(const bool aIsUsingPBR);
	public:
		bool GetShouldRenderMesh() const;
		bool GetShouldRenderDebugLines() const;
		bool GetShouldRenderBoundingBox() const;
		bool GetShouldRenderSkeletonLines() const;
		bool GetIsUsingPBR() const;
	private:
		void RenderModel(const Math::Matrix4x4f& aTransformMatrix, const Graphics::Mesh* aMesh, ID3D11DeviceContext* aContext) const;
		void BindTextures(const ECS::MeshComponent* aMeshComponent, ID3D11DeviceContext* aContext) const;
		void UpdateJointTransforms(const Math::Matrix4x4f* aJointMatrices) const;
		void LoadSettingsFromJson();
	private:
		const bool CreateObjectBuffer();
		const bool CreateBoneBuffer();
		const bool CreateInstanceBuffer();
	private:
		std::vector<Line> myDebugLines;
		std::vector<Sphere> myDebugSpheres;
		std::vector<BoundingBox3DData> myBoundingBoxesData;

		std::unique_ptr<LineDrawer> myLineDrawer;
		std::unique_ptr<SphereDrawer> mySphereDrawer;
		std::unique_ptr<SpriteDrawer> mySpriteDrawer;
		std::unique_ptr<BoundingBoxDrawer> myBoundingBoxDrawer;

		std::unique_ptr<Graphics::ConstantBuffer> myTransformBuffer;
		std::unique_ptr<Graphics::ConstantBuffer> myJointBuffer;

		Microsoft::WRL::ComPtr<ID3D11Buffer> myInstanceBuffer;

		bool myIsUsingPBR;
		bool myShouldRenderMesh;
		bool myShouldRenderDebugLines;
		bool myShouldRenderBoundingBox;
		bool myShouldRenderSkeletonLines;
	};
}