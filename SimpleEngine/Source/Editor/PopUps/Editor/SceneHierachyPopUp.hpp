#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class SceneHierachyPopUp final : public PopUp
	{
	public:
		SceneHierachyPopUp(const std::string& aName);

		void Render() override;
	private:
		void ShowAddButtons();
		void ShowActiveSceneName();
		void ShowSceneEntities();
	};
}
