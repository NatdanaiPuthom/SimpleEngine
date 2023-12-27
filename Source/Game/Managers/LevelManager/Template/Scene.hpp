#pragma once

class ModelInstance;

class Scene
{
public:
	virtual ~Scene() = default;

	virtual void Init() {};
	virtual void Update() = 0;

	std::vector<std::shared_ptr<ModelInstance>> myModelInstances;
};
