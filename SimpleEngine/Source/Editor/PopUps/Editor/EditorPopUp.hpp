#pragma once
#include "Editor/Core/PopUp.hpp"
#include "Editor/Command/Commands/SetEntityTransformCommand.hpp"
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
	class CommandTracker;
}

namespace Editor
{
	class EditorPopUp final : public PopUp
	{
	public:
		EditorPopUp(const std::string& aName, CommandTracker* aCommandTracker);

		void Init() override;
		void Render() override;

		void RemoveMeLater();
		void RemoveMeLaterLater();
	private:
		void ShowEntityTransformGizmo();
		void ShowInspector(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowSceneHierachy(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowComponents(ECS::Entity& aSelectedEntity, ECS::EntityComponentSystem& aActiveECS) const;
		void ShowSceneEntities(std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowActiveSceneName();
		void RemoveEntity(std::vector<ECS::Entity>& aEntities, int& aSelected);
		void ShowAddPopUps(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected);
	private:
		std::unique_ptr<ECS::EntityComponentSystem> myTemporaryECSEditor;
		CommandTracker* myCommandTracker = nullptr;
		SetEntityTransformCommand mySetEntityTransformCommand;
		bool myIsDraggingEntity = false;
		bool myShowAdvanced = false;
	};
}
