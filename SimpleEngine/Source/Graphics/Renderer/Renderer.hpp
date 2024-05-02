#pragma once
#include "Graphics/Renderer/Drawer/BoundingBoxDrawer.hpp"
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/Renderer/Drawer/SphereDrawer.hpp"
#include "Graphics/Renderer/Drawer/SpriteDrawer.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/AnimatedModel.hpp"

#include <memory>
#include <vector>

namespace ECS
{
	struct MeshComponent;
	struct TransformComponent;
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

		void TestRender(ECS::TransformComponent* aTransformComponent, ECS::MeshComponent* aMeshComponent);

		Renderer();
		~Renderer();

		void Init();

		void RenderModel(const std::shared_ptr<const Graphics::Model> aModel) const;
		void RenderModel(const std::shared_ptr<const Graphics::AnimatedModel> aModel) const;
		void RenderModel(const Graphics::Model& aModel) const;
		void RenderModel(const Graphics::AnimatedModel& aModel) const;

		void RenderBoundingBox(const std::shared_ptr<const Graphics::Model> aModel) const;
		void RenderBoundingBox(const std::shared_ptr<const Graphics::AnimatedModel> aModel) const;
		void RenderBoundingBox(const Graphics::Model& aModel) const;
		void RenderBoundingBox(const Graphics::AnimatedModel& aModel) const;

		void RenderLine(const Drawer::Line& aLine);
		void RenderLine(const std::vector<Drawer::Line> aLines);

		void RenderSphere(const Drawer::Sphere& aSphere);
		void RenderSprite2D(const Drawer::Sprite2D& aSprite);

		void RenderAnimatedSkeletonLines(const Graphics::AnimatedModel& aModel, const Graphics::ModelSpacePose& aLocalPose);
		void RenderAnimatedSkeletonLines(const std::shared_ptr<const Graphics::AnimatedModel> aModel, const Graphics::ModelSpacePose& aLocalPose);
		void RenderStaticSkeletonLines(const Graphics::AnimatedModel& aModel);
		void RenderStaticSkeletonLines(const std::shared_ptr<const Graphics::AnimatedModel> aModel);

		void RenderEverythingUpSideDown() const;
		void RenderPlaneReflection(const std::shared_ptr<const Graphics::Model> aModel) const;
		void RenderRefraction() const;

		void TestIKSkeletonLines(const Graphics::AnimatedModel& aModel);

	public:
		bool IsDebugModeOn() const;
		void SetDebugMode(const bool aSetDebugMode);
	private:
		const bool CreateObjectBuffer();
		const bool CreateBoneBuffer();
		void LoadSettingsFromJson();
		void RenderUpSideDown(const std::shared_ptr<const Graphics::Model> aModel) const;
		void RenderRefraction(const std::shared_ptr<const Graphics::Model> aModel) const;
	private:
		std::vector<Drawer::Line> myAnimatedSkeletonLines;
		std::vector<Drawer::Line> myStaticSkeletonLines;

		std::unique_ptr<Drawer::BoundingBoxDrawer> myBoundingBoxDrawer;
		std::unique_ptr<Drawer::LineDrawer> myLineDrawer;
		std::unique_ptr<Drawer::SphereDrawer> mySphereDrawer;
		std::unique_ptr<Drawer::SpriteDrawer> mySpriteDrawer;

		std::unique_ptr<Graphics::ConstantBuffer> myObjectBuffer;
		std::unique_ptr<Graphics::ConstantBuffer> myBoneBuffer;

		bool myDebugMode;
	};
}