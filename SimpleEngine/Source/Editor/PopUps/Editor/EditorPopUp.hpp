#pragma once
#include "Editor/Core/PopUp.hpp"
#include <memory>
#include <string>
#include <vector>

namespace ECS
{
	class EntityComponentSystem;
	class Entity;
}

namespace Editor
{
	class EditorPopUp final : public PopUp
	{
	public:
		EditorPopUp(const std::string& aName);

		void Init() override;
		void Render() override;

		void RemoveMeLater();
		void RemoveMeLaterLater();
	private:
		void ShowInspector(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowSceneHierachy(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowComponents(ECS::Entity& aSelectedEntity, ECS::EntityComponentSystem& aActiveECS) const;
		void ShowSceneEntities(std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowActiveSceneName();
		void RemoveEntity(std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowAddPopUps(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected);
	private:
		std::unique_ptr<ECS::EntityComponentSystem> myTemporaryECSEditor;
		bool myShowAdvanced;
	};
}
