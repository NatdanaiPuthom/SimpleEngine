#pragma once

class ModelInstance;
class ConstantBuffer;

class Renderer final
{
public:
	Renderer();
	~Renderer();

	void SetModelBuffer(std::vector<ModelInstance*> aModelBuffer);
	void Render(const ModelInstance& aModelInstance);
public:
	std::vector<ModelInstance*> GetAllModelInstances();
private:
	const bool CreateObjectBuffer();
private:
	std::vector<ModelInstance*> myModelBuffer;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
};
