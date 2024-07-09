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
		void ShowInspector(ECS::EntityComponentSystem& aActiveECS, ECS::Entities& aEntities, int& aSelected);
		void ShowSceneHierachy(ECS::EntityComponentSystem& aActiveECS, ECS::Entities& aEntities, int& aSelected);

		void ShowActiveSceneName();
		void ShowSceneEntities(ECS::Entities& aEntities, int& aSelected);
		void ShowAddPopUps(ECS::EntityComponentSystem& aActiveECS, ECS::Entities& aEntities, int& aSelected);
		void ShowComponents(ECS::Entity aSelectedEntity, ECS::EntityComponentSystem& aActiveECS) const;

		void RemoveEntity(ECS::Entities& aEntities, int& aSelected);
	private:
		bool myShowAdvanced;
	};
}