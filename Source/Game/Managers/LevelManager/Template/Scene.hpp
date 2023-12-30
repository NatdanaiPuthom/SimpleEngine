#pragma once

class ModelInstance;
class DirectionalLightVisual;

class Scene
{
public:
	Scene()
	{
		myDirectionalLight = std::make_unique<DirectionalLightVisual>();
		myModelInstances.push_back(myDirectionalLight->myModel);
	}

	virtual ~Scene() = default;

	virtual void Init() {};

	virtual void Update()
	{
		myDirectionalLight->Update();
	}

	virtual void Render()
	{
		Renderer* renderer = SimpleGlobal::GetRenderer();

		for (const auto& model : myModelInstances)
		{
			renderer->RenderModel(model);
		}

		if (renderer->IsDebugModeOn())
		{
			for (const auto& model : myModelInstances)
			{
				renderer->RenderBoundingBox(model);
			}

			renderer->RenderLine(*myDirectionalLight->myLine);
		}
	};

	std::vector<std::shared_ptr<ModelInstance>> myModelInstances;
	std::unique_ptr<DirectionalLightVisual> myDirectionalLight;
};