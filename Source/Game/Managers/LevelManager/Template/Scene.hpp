#pragma once

class ModelInstance;
class DirectionalLightVisual;

class Scene
{
public:
	Scene()
	{
		myDirectionalLight = std::make_unique<DirectionalLightVisual>();
	}

	virtual ~Scene() = default;

	virtual void Init() {};
	virtual void Update() = 0;

	std::vector<std::shared_ptr<ModelInstance>> myModelInstances;
	std::unique_ptr<DirectionalLightVisual> myDirectionalLight;
};