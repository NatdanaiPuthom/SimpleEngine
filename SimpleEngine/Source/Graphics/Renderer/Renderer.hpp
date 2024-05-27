#pragma once
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
	struct AnimatedComponent;
}

namespace Graphics
{
	class ConstantBuffer;
}

namespace Drawer
{
	class Renderer final
	{
	public:
		Renderer();
		~Renderer();

		void Init();

		void RenderAnimatedModel(ECS::TransformComponent* aTransformComponent, ECS::MeshComponent* aMeshComponent, ECS::AnimatedComponent* aSkeletonComponent);
		void RenderStaticModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent) const;

		void RenderLine(const Drawer::Line& aLine);
		void RenderLine(const std::vector<Drawer::Line>& aLines);
		void RenderSphere(const Drawer::Sphere& aSphere);
		void RenderSprite2D(const Drawer::Sprite2D& aSprite);

	private:
		const bool CreateObjectBuffer();
		const bool CreateBoneBuffer();
		void LoadSettingsFromJson();
	private:
		std::unique_ptr<Drawer::LineDrawer> myLineDrawer;
		std::unique_ptr<Drawer::SphereDrawer> mySphereDrawer;
		std::unique_ptr<Drawer::SpriteDrawer> mySpriteDrawer;
		std::unique_ptr<Drawer::BoundingBoxDrawer> myBoundingBoxDrawer;

		std::unique_ptr<Graphics::ConstantBuffer> myTransformBuffer;
		std::unique_ptr<Graphics::ConstantBuffer> myJointBuffer;
	};
}