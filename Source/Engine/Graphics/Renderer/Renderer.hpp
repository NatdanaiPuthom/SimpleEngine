#pragma once

class ModelInstance;
class ConstantBuffer;
class BoundingBoxDrawer;
class LineDrawer;
class SphereDrawer;

namespace Drawer
{
	struct Line;
	struct Sphere;
}

class Renderer final
{
public:
	Renderer();
	~Renderer();

	void RenderModel(const std::shared_ptr<const ModelInstance> aModelInstance) const;
	void RenderLine(const Drawer::Line& aLine);
	void RenderSphere(const Drawer::Sphere& aSphere);
	void RenderBoundingBox(const std::shared_ptr<const ModelInstance> aModelInstance) const;

	void RenderEverythingUpSideDown(const ModelInstance* const aModelInstance) const;
	void RenderPlaneReflection(const ModelInstance* const aModelInstance) const;

	bool IsDebugModeOn() const;
public:
	void SetDebugMode(const bool aSetDebugMode);
private:
	const bool CreateObjectBuffer();
	void LoadSettingsFromJson();
private:
	std::unique_ptr<BoundingBoxDrawer> myBoundingBoxDrawer;
	std::unique_ptr<LineDrawer> myLineDrawer;
	std::unique_ptr<SphereDrawer> mySphereDrawer;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;

	bool myDebugMode;
};
