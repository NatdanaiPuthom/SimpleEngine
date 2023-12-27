#pragma once

class ModelInstance;
class ConstantBuffer;
class BoundingBoxDrawer;
class LineDrawer;

namespace Drawer
{
	struct Line;
}

class Renderer final
{
public:
	Renderer();
	~Renderer();

	void RenderModel(const std::shared_ptr<const ModelInstance> aModelInstance) const;
	void RenderLine(const Drawer::Line& aLine);
	void RenderBoundingBox(const std::shared_ptr<const ModelInstance> aModelInstance) const;

	void RenderEverythingUpSideDown(const ModelInstance* const aModelInstance) const;
	void RenderPlaneReflection(const ModelInstance* const aModelInstance) const;
public:
	void SetModelBuffer(std::vector<ModelInstance*>& aModelBuffer);
	void SetDebugMode(const bool aSetDebugMode);
	void SetBoundingBoxLineColor(const SimpleUtilities::Vector3f& aColor);
public:
	bool IsDebugModeOn() const;
	std::vector<ModelInstance*> GetAllModelInstances();
private:
	const bool CreateObjectBuffer();
	void LoadSettingsFromJson();
private:
	std::vector<ModelInstance*> myModelBuffer;

	std::unique_ptr<BoundingBoxDrawer> myBoundingBoxDrawer;
	std::unique_ptr<LineDrawer> myLineDrawer;

	std::unique_ptr<ConstantBuffer> myObjectBuffer;

	bool myDebugMode;
};
