#pragma once
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Special/DirectionalLightVisual.hpp"
#include <memory>
#include <vector>

namespace Graphics
{
	class Model;
	class DirectionalLightVisual;
}

namespace Simple
{
	class Scene
	{
	public:
		std::unique_ptr<Graphics::DirectionalLightVisual> myDirectionalLight;
		std::vector<std::shared_ptr<Graphics::Model>> myModels;

	public:
		Scene()
		{
			myDirectionalLight = std::make_unique<Graphics::DirectionalLightVisual>();
			myModels.push_back(myDirectionalLight->myModel);
		}

		virtual ~Scene() = default;

		virtual void OnEnter() {};
		virtual void OnExit() {};

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

				myDirectionalLight->Render();
			}
		};
	};
}