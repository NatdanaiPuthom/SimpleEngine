#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/SceneHierachyPopUp.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneHierachyPopUp::SceneHierachyPopUp(const std::string& aName)
		: PopUp(aName)
	{
	}

	void SceneHierachyPopUp::Render()
	{
		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			//ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();

			ShowAddButtons();

			ImGui::Separator();

			ShowActiveSceneName();

			ImGui::Separator();
		}

		ImGui::End();
	}

	void SceneHierachyPopUp::ShowAddButtons()
	{
		static const std::string addButton = "Add" + myImGuiTag;
		static const std::string addSceneObjectButton = "Add Scene Object" + myImGuiTag;


		if (ImGui::Button(addButton.c_str()))
		{
			ImGui::OpenPopup(addSceneObjectButton.c_str());
		}

		if (ImGui::BeginPopup(addSceneObjectButton.c_str()))
		{
			static const std::string addEntityItem = "Add Entity" + myImGuiTag;
			static const std::string addCubeItem = "Add Cube" + myImGuiTag;

			if (ImGui::MenuItem(addEntityItem.c_str()))
			{
			}

			if (ImGui::MenuItem(addCubeItem.c_str()))
			{
			}

			ImGui::EndPopup();
		}
	}

	void SceneHierachyPopUp::ShowActiveSceneName()
	{
		static const std::string sceneInputWithTextButton = "Scene" + myImGuiTag;

		const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();

		char sceneName[256]{};
		memset(sceneName, '\0', sizeof(sceneName));
		strncpy_s(sceneName, sceneInfo->name.c_str(), sizeof(sceneName));
		sceneName[sizeof(sceneName) - 1] = '\0';

		if (ImGui::InputTextWithHint(sceneInputWithTextButton.c_str(), "Name", sceneName, sizeof(sceneName)))
		{
			if (ImGui::IsItemFocused() && MainSingleton::GetInputManager().IsKeyPressed(VK_RETURN))
			{
				std::string newSceneName(sceneName);
				MainSingleton::GetSceneManager().ChangeSceneName(newSceneName);
			}
		}
	}

	void SceneHierachyPopUp::ShowSceneEntities()
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.18f, 0.18f, 0.18f, 0.80f).Value);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

		const ImVec2 parentSize = ImGui::GetContentRegionAvail();
		static const std::string listBoxName = "" + myImGuiTag;

		if (ImGui::BeginListBox("##SceneEntities", parentSize))
		{
			ImGui::EndListBox();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
}
