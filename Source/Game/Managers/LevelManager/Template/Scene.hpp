#pragma once

class Model;
class DirectionalLightVisual;

class Scene
{
public:
	Scene()
	{
		myDirectionalLight = std::make_unique<DirectionalLightVisual>();
		myModels.push_back(myDirectionalLight->myModel);
	}

	virtual ~Scene() = default;

	virtual void Init() {};

	virtual void Update()
	{
		myDirectionalLight->Update();
	}

	virtual void Render()
	{
		Simple::Renderer* renderer = SimpleGlobal::GetRenderer();

		for (const auto& model : myModels)
		{
			renderer->RenderModel(model);
		}

		if (renderer->IsDebugModeOn())
		{
			for (const auto& model : myModels)
			{
				renderer->RenderBoundingBox(model);
			}

			renderer->RenderLine(*myDirectionalLight->myLine);
		}
	};

	std::vector<std::shared_ptr<Model>> myModels;
	std::unique_ptr<DirectionalLightVisual> myDirectionalLight;
};