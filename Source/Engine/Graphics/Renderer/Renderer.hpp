#pragma once

class Model;
class ConstantBuffer;
class BoundingBoxDrawer;
class LineDrawer;
class SphereDrawer;

namespace Drawer
{
	struct Line;
	struct Sphere;
}

namespace Simple
{
	class Renderer final
	{
	public:
		Renderer();
		~Renderer();

		void RenderModel(const std::shared_ptr<const Model> aModelInstance) const;
		void RenderLine(const Drawer::Line& aLine);
		void RenderSphere(const Drawer::Sphere& aSphere);
		void RenderBoundingBox(const std::shared_ptr<const Model> aModelInstance) const;

		void RenderEverythingUpSideDown() const;
		void RenderPlaneReflection(const Model* const aModelInstance) const;

		bool IsDebugModeOn() const;
	public:
		void SetDebugMode(const bool aSetDebugMode);
	private:
		const bool CreateObjectBuffer();
		void LoadSettingsFromJson();
		void RenderUpSideDown(const Model* const aModelInstance) const;
	private:
		std::unique_ptr<BoundingBoxDrawer> myBoundingBoxDrawer;
		std::unique_ptr<LineDrawer> myLineDrawer;
		std::unique_ptr<SphereDrawer> mySphereDrawer;
		std::unique_ptr<ConstantBuffer> myObjectBuffer;

		bool myDebugMode;
	};
}