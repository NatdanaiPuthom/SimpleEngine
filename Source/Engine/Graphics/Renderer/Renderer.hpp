#pragma once

class LineDrawer;
class SphereDrawer;

namespace Drawer
{
	struct Line;
	struct Sphere;
}

namespace Simple
{
	class ConstantBuffer;
	class Model;
	class BoundingBoxDrawer;
}

namespace Simple
{
	class Renderer final
	{
	public:
		Renderer();
		~Renderer();

		void RenderModel(const std::shared_ptr<const Simple::Model> aModel) const;
		void RenderLine(const Drawer::Line& aLine);
		void RenderSphere(const Drawer::Sphere& aSphere);
		void RenderBoundingBox(const std::shared_ptr<const Simple::Model> aModel) const;

		void RenderEverythingUpSideDown() const;
		void RenderPlaneReflection(const std::shared_ptr<const Simple::Model> aModel) const;
		void RenderRefraction() const;

		bool IsDebugModeOn() const;
	public:
		void SetDebugMode(const bool aSetDebugMode);
	private:
		const bool CreateObjectBuffer();
		void LoadSettingsFromJson();
		void RenderUpSideDown(const std::shared_ptr<const Simple::Model> aModel) const;
		void RenderRefraction(const std::shared_ptr<const Simple::Model> aModel) const;
	private:
		std::unique_ptr<Simple::BoundingBoxDrawer> myBoundingBoxDrawer;
		std::unique_ptr<LineDrawer> myLineDrawer;
		std::unique_ptr<SphereDrawer> mySphereDrawer;
		std::unique_ptr<Simple::ConstantBuffer> myObjectBuffer;

		bool myDebugMode;
	};
}