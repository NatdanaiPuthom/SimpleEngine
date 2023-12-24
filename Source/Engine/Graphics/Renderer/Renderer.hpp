#pragma once

class ModelInstance;
class ConstantBuffer;

class Renderer final
{
public:
	Renderer();
	~Renderer();

	void Render(const ModelInstance& aModelInstance);
public:
	std::vector<ModelInstance*> GetAllModelInstances();
private:
	const bool CreateObjectBuffer();
private:
	std::vector<ModelInstance*> myModelInstances;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
};
