#pragma once
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Special/DirectionalLightVisual.hpp"
#include <memory>
#include <vector>
#include <string>

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
		std::string mySceneName;
		std::unique_ptr<Graphics::DirectionalLightVisual> myDirectionalLight;
		std::vector<std::shared_ptr<Graphics::Model>> myModels;

	public:
		Scene()
		{
			static size_t currentSceneID = 0;
			mySceneName = "Scene " + std::to_string(currentSceneID);
			currentSceneID++;
			myDirectionalLight = std::make_unique<Graphics::DirectionalLightVisual>();
			myModels.push_back(myDirectionalLight->myModel);
		}

		virtual ~Scene() = default;

		void SetSceneName(const std::string& aSceneName)
		{
			mySceneName = aSceneName;
		}

		const std::string& GetSceneName() const
		{
			return mySceneName;
		}

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