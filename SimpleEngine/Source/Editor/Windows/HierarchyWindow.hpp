#pragma once
#include "Editor/Template/ToolInterface.hpp"

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
		void ShowInspector(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::IEntity>& aEntities, int& aSelected);
		void ShowSceneHierachy(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::IEntity>& aEntities, int& aSelected);

		void ShowActiveSceneName();
		void ShowSceneEntities(std::vector<ECS::IEntity>& aEntities, int& aSelected);
		void ShowAddPopUps(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::IEntity>& aEntities, int& aSelected);
		void ShowComponents(ECS::IEntity& aSelectedEntity, ECS::EntityComponentSystem& aActiveECS) const;

		void RemoveEntity(std::vector<ECS::IEntity>& aEntities, int& aSelected);
	private:
		bool myShowAdvanced;
	};
}