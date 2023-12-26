#pragma once

class ModelInstance;
class ConstantBuffer;

class PlaneReflection;

class Renderer final
{
public:
	Renderer();
	~Renderer();

	void Render(const ModelInstance* const aModelInstance) const;
	void RenderPlane(PlaneReflection* aPlaneReflection);
public:
	void SetModelBuffer(std::vector<ModelInstance*>& aModelBuffer);
	std::vector<ModelInstance*> GetAllModelInstances();
private:
	const bool CreateObjectBuffer();
private:
	std::vector<ModelInstance*> myModelBuffer;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
};
