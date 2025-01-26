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
		ECS::Entity* GetSelectedEntity() const;
	private:
		void ShowEntityTransformGizmo();
		void ShowInspector(ECS::EntityComponentSystem& aActiveECS);
		void ShowSceneHierachy(ECS::EntityComponentSystem& aActiveECS);
		void ShowComponents(ECS::Entity& aSelectedEntity, ECS::EntityComponentSystem& aActiveECS) const;
		void ShowSceneEntities();
		void ShowActiveSceneName();
		void RemoveSelectedEntity();
		void RemoveEntity(size_t aEntityIndex);
		ECS::EntityID CreateEntity();
		void ShowAddPopUps(ECS::EntityComponentSystem& aActiveECS);
		void SelectEntity(size_t aNewIndex);
	private:
		std::unique_ptr<ECS::EntityComponentSystem> myTemporaryECSEditor;
		std::vector<ECS::EntityID> myVisibleEntityIDs;
		CommandTracker* myCommandTracker = nullptr;
		size_t mySelectedEntityIndex = 0;
		SetEntityTransformCommand mySetEntityTransformCommand;
		bool myIsDraggingEntity = false;
		bool myShowAdvanced = false;
	};
}
