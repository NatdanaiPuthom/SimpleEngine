#pragma once
#include "Graphics/Renderer/Drawer/BoundingBoxDrawer.hpp"
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/Renderer/Drawer/SphereDrawer.hpp"
#include "Graphics/Renderer/Drawer/SpriteDrawer.hpp"
#include "Graphics/Model/Model.hpp"

#include <memory>
#include <vector>

namespace Simple
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

		void RenderModel(const std::shared_ptr<const Simple::Model> aModel) const;
		void RenderModel(const Simple::Model& aModel) const;
		void RenderBoundingBox(const std::shared_ptr<const Simple::Model> aModel) const;

		void RenderLine(const Drawer::Line& aLine);
		void RenderSphere(const Drawer::Sphere& aSphere);
		void RenderSprite2D(const Drawer::Sprite2D& aSprite);

		void RenderAnimatedSkeletonLines(const Simple::Model& aModel, const Simple::LocalSpacePose& aLocalPose);
		void RenderAnimatedSkeletonLines(const std::shared_ptr<const Simple::Model> aModel, const Simple::LocalSpacePose& aLocalPose);
		void RenderStaticSkeletonLines(const Simple::Model& aModel);
		void RenderStaticSkeletonLines(const std::shared_ptr<const Simple::Model> aModel);

		void RenderLineInstance(const std::vector<Drawer::Line> aLines);

		void RenderEverythingUpSideDown() const;
		void RenderPlaneReflection(const std::shared_ptr<const Simple::Model> aModel) const;
		void RenderRefraction() const;

	public:
		bool IsDebugModeOn() const;
		void SetDebugMode(const bool aSetDebugMode);
	private:
		const bool CreateObjectBuffer();
		const bool CreateBoneBuffer();
		void LoadSettingsFromJson();
		void RenderUpSideDown(const std::shared_ptr<const Simple::Model> aModel) const;
		void RenderRefraction(const std::shared_ptr<const Simple::Model> aModel) const;
	private:
		std::vector<Drawer::Line> myAnimatedSkeletonLines;
		std::vector<Drawer::Line> myStaticSkeletonLines;

		std::unique_ptr<Drawer::BoundingBoxDrawer> myBoundingBoxDrawer;
		std::unique_ptr<Drawer::LineDrawer> myLineDrawer;
		std::unique_ptr<Drawer::SphereDrawer> mySphereDrawer;
		std::unique_ptr<Drawer::SpriteDrawer> mySpriteDrawer;

		std::unique_ptr<Simple::ConstantBuffer> myObjectBuffer;
		std::unique_ptr<Simple::ConstantBuffer> myBoneBuffer;

		bool myDebugMode;
	};
}