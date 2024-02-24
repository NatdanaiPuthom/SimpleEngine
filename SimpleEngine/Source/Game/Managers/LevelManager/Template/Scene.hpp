#pragma once
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Special/DirectionalLightVisual.hpp"
#include <memory>
#include <vector>

namespace Simple
{
	class Model;
	class DirectionalLightVisual;
}

namespace Simple
{
	class Scene
	{
	public:
		Scene()
		{
			myDirectionalLight = std::make_unique<Simple::DirectionalLightVisual>();
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
			Drawer::Renderer* renderer = Global::GetRenderer();

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

		std::vector<std::shared_ptr<Simple::Model>> myModels;
		std::unique_ptr<Simple::DirectionalLightVisual> myDirectionalLight;
	};
}