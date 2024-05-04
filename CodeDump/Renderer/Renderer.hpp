#pragma once
#include "Graphics/Renderer/Drawer/BoundingBoxDrawer.hpp"
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/Renderer/Drawer/SphereDrawer.hpp"
#include "Graphics/Renderer/Drawer/SpriteDrawer.hpp"
#include "Graphics/Model/Model.hpp"
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
		void TestIKSkeletonLines(const Graphics::AnimatedModel& aModel);

		void TestRender(ECS::TransformComponent* aTransformComponent, ECS::MeshComponent* aMeshComponent);
		void TestRenderAnimated(ECS::TransformComponent* aTransformComponent, ECS::MeshComponent* aMeshComponent,ECS::AnimatedComponent* aSkeletonComponent);

		Renderer();
		~Renderer();

		void Init();

		void RenderModel(const std::shared_ptr<const Graphics::Model> aModel) const;

		void RenderBoundingBox(const std::shared_ptr<const Graphics::Model> aModel) const;
		void RenderBoundingBox(const std::shared_ptr<const Graphics::AnimatedModel> aModel) const;
		void RenderBoundingBox(const Graphics::Model& aModel) const;
		void RenderBoundingBox(const Graphics::AnimatedModel& aModel) const;

		void RenderLine(const Drawer::Line& aLine);
		void RenderLine(const std::vector<Drawer::Line> aLines);

		void RenderSphere(const Drawer::Sphere& aSphere);

		void RenderSprite2D(const Drawer::Sprite2D& aSprite);

		bool IsDebugModeOn() const;
		void SetDebugMode(const bool aSetDebugMode);

	private:
		const bool CreateObjectBuffer();
		const bool CreateBoneBuffer();
		void LoadSettingsFromJson();
	private:
		std::unique_ptr<Drawer::LineDrawer> myLineDrawer;
		std::unique_ptr<Drawer::SphereDrawer> mySphereDrawer;
		std::unique_ptr<Drawer::SpriteDrawer> mySpriteDrawer;
		std::unique_ptr<Drawer::BoundingBoxDrawer> myBoundingBoxDrawer;

		std::unique_ptr<Graphics::ConstantBuffer> myObjectBuffer;
		std::unique_ptr<Graphics::ConstantBuffer> myBoneBuffer;

		bool myDebugMode;
	};
}