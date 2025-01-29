#pragma once
#include "Editor/Core/PopUp.hpp"
#include "Editor/Tools/TransformEntityTool.hpp"
#include "Editor/Tools/GridSnapTool.hpp"
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
	private:
		void ShowInspector(ECS::EntityComponentSystem& aActiveECS);
		void ShowComponents(ECS::Entity& aSelectedEntity, ECS::EntityComponentSystem& aActiveECS) const;
	private:
		std::unique_ptr<ECS::EntityComponentSystem> myTemporaryECSEditor;
		std::vector<size_t> myVisibleEntityIDs;
		CommandTracker* myCommandTracker = nullptr;
		size_t mySelectedEntityIndex = 0;
		bool myShowAdvanced = false;
		TransformEntityTool myTransformEntityTool;
		GridSnapTool myGridSnapTool;
	};
}
