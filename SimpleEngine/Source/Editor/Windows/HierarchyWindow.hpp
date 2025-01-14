#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include <memory>

namespace ECS
{
	class EntityComponentSystem;
}

namespace Editor
{
	class HierarchyWindow final : public Editor::ToolInterface
	{
	public:
		HierarchyWindow();

		void Init() override;
		void Update() override;
		void Draw() override;
	private:
		void ShowInspector(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowSceneHierachy(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected);

		void ShowActiveSceneName();
		void ShowSceneEntities(std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowAddPopUps(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowComponents(ECS::Entity& aSelectedEntity, ECS::EntityComponentSystem& aActiveECS) const;

		void RemoveEntity(std::vector<ECS::Entity>& aEntities, int& aSelected);
	private:
		std::unique_ptr<ECS::EntityComponentSystem> myTemporaryECSEditor;
		bool myShowAdvanced;
	};
}