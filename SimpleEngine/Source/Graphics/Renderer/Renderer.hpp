#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Renderer/Drawer/BoundingBoxDrawer.hpp"
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/Renderer/Drawer/SphereDrawer.hpp"
#include "Graphics/Renderer/Drawer/SpriteDrawer.hpp"
#include <memory>
#include <vector>

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

namespace Drawer
{
	class Renderer final
	{
	public:
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
		void Push(const std::vector<Drawer::Line>& aLines);
		void RenderStaticSkeletonLines(const ECS::TransformComponent* aTransformComponent, const ECS::AnimationComponent* aAnimationPlayerComponent);
		void RenderAnimatedSkeletonLines(const ECS::TransformComponent* aTransformComponent, const ECS::AnimationComponent* aAnimationPlayerComponent);
		void RenderDebugLines();
	public:
		void SetShouldRenderMesh(const bool aShouldRender);
		void SetShouldRenderDebugLines(const bool aShouldRender);
		void SetIsUsingPBR(const bool aIsUsingPBR);
	public:
		bool GetShouldRenderMesh() const;
		bool GetShouldRenderDebugLines() const;
		bool GetIsUsingPBR() const;
	private:
		void RenderModel(const Math::Matrix4x4f& aTransformMatrix, const Graphics::Mesh* aMesh, ID3D11DeviceContext* aContext) const;
		void BindTextures(const ECS::MeshComponent* aMeshComponent, ID3D11DeviceContext* aContext) const;
		void UpdateJointTransforms(const Math::Matrix4x4f* aJointMatrices) const;
	private:
		const bool CreateObjectBuffer();
		const bool CreateBoneBuffer();
	private:
		std::vector<Drawer::Line> myDebugLines;
		std::vector<Drawer::Sphere> myDebugSpheres;

		std::unique_ptr<Drawer::LineDrawer> myLineDrawer;
		std::unique_ptr<Drawer::SphereDrawer> mySphereDrawer;
		std::unique_ptr<Drawer::SpriteDrawer> mySpriteDrawer;
		std::unique_ptr<Drawer::BoundingBoxDrawer> myBoundingBoxDrawer;

		std::unique_ptr<Graphics::ConstantBuffer> myTransformBuffer;
		std::unique_ptr<Graphics::ConstantBuffer> myJointBuffer;

		bool myShouldRenderMesh;
		bool myIsUsingPBR;
		bool myShouldRenderDebugLines;
	};
}