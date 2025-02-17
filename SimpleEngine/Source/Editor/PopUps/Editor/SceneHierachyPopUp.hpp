#pragma once
#include "Editor/Core/PopUp.hpp"
#include <memory>
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
	class SceneHierachyPopUp final : public PopUp
	{
	public:
		SceneHierachyPopUp(const std::string& aName, CommandTracker* aCommandTracker);

		void Render() override;
	private:
		void ShowAddButtons();
		void ShowActiveSceneName();
		void ShowSceneEntities();

		size_t CreateEntity();
		void SelectEntity(size_t aNewIndex);
		void RemoveEntity(size_t aEntityIndex);
		void RemoveSelectedEntity();

		ECS::Entity* GetSelectedEntity() const;
	private:
		CommandTracker* myCommandTracker;
	};
}
