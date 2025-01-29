#pragma once
#include "Editor/Core/PopUp.hpp"
#include "Engine/ECS/ECS.hpp"

namespace Editor
{
	class SceneInspectorPopUp final : public PopUp
	{
	public:
		SceneInspectorPopUp(const std::string& aName);

		void Render() override;
	private:
		void ShowInspector();
		void ShowComponents(ECS::Entity& aSelectedEntity, ECS::EntityComponentSystem& aActiveECS);
	private:
		bool myShowAdvanced;
	};
}
