#pragma once

class ModelInstance;
class ConstantBuffer;
class BoundingBoxDrawer;

class Renderer final
{
public:
	Renderer();
	~Renderer();

	void Render(const std::shared_ptr<const ModelInstance> aModelInstance) const;
	void RenderBoundingBox(const std::shared_ptr<const ModelInstance> aModelInstance) const;

	void RenderEverythingUpSideDown(const ModelInstance* const aModelInstance) const;
	void RenderPlaneReflection(const ModelInstance* const aModelInstance) const;
public:
	void SetModelBuffer(std::vector<ModelInstance*>& aModelBuffer);
	void SetDebugMode(const bool aSetDebugMode);
public:
	bool IsDebugModeOn() const;
	std::vector<ModelInstance*> GetAllModelInstances();
private:
	const bool CreateObjectBuffer();
	void LoadSettingsFromJson();
private:
	std::vector<ModelInstance*> myModelBuffer;
	std::unique_ptr<BoundingBoxDrawer> myBoundingBoxDrawer;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;

	bool myDebugMode;
};
