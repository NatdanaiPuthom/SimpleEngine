#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/EditorPopUp.hpp"

#include "Engine/ECS/Components/AllEngineComponents.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "Editor/Editor.hpp"

namespace Editor
{
	EditorPopUp::EditorPopUp(const std::string& aName) : PopUp(aName)
		, myShowAdvanced(false)
	{
	}

	void EditorPopUp::Init()
	{
		myTemporaryECSEditor = std::make_unique<ECS::EntityComponentSystem>();
		myTemporaryECSEditor->Init();
	}

	void EditorPopUp::Render()
	{
		static int selected = 0;

		ECS::EntityComponentSystem& activeECS = MainSingleton::GetSceneManager().GetCurrentECS();
		std::vector<ECS::Entity>& entities = activeECS.GetAllEntities();

		ShowSceneHierachy(activeECS, entities, selected);

		if (selected < 0)
		{
			selected = 0;
		}
		else if (selected >= static_cast<int>(entities.size()))
		{
			selected = static_cast<int>(entities.size() - 1);
		}

		if (entities.empty() == false)
		{
			EditorEngine::mySelectedEntityID = entities[selected].GetID();
		}
		else
		{
			EditorEngine::mySelectedEntityID = static_cast<size_t>(-1);
		}

		ShowInspector(activeECS, entities, selected);

		if (MainSingleton::GetInputManager().IsKeyPressed(VK_DELETE))
		{
			if (entities.size() > 0)
			{
				if (selected >= 0)
				{
					RemoveEntity(entities, selected);
					//EditorEngine::mySelectedEntityID = entities[selected].GetID();
					EditorEngine::mySelectedEntityID = static_cast<size_t>(-1);
					return;
				}
			}
		}

		if (MainSingleton::GetInputManager().IsKeyPressed('K'))
		{
			if (entities.size() > 0)
			{
				if (selected >= 0)
				{
					entities[selected].DuplicateThis();
				}
			}
		}

		static ECS::EntityID tempEntityID = static_cast<size_t>(-1);

		if (MainSingleton::GetInputManager().IsKeyHeld(VK_CONTROL) && MainSingleton::GetInputManager().IsKeyPressed('C'))
		{
			if (entities.size() > 0)
			{
				if (selected >= 0)
				{
					tempEntityID = entities[selected].DuplicateThisToAnotherECS(myTemporaryECSEditor->GetEntityManager());
				}
			}
		}
		else if (MainSingleton::GetInputManager().IsKeyHeld(VK_CONTROL) && MainSingleton::GetInputManager().IsKeyPressed('V'))
		{
			if (entities.size() > 0)
			{
				if (selected >= 0)
				{
					if (tempEntityID != static_cast<size_t>(-1))
					{
						ECS::Entity& tempEntity = myTemporaryECSEditor->GetEntity(tempEntityID);
						tempEntity.DuplicateThisToAnotherECS(activeECS.GetEntityManager());
					}
				}
			}
		}
	}

	void EditorPopUp::ShowInspector(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected)
	{
		if (ImGui::Begin("Inspector##HierachyWindow"))
		{
			if (aEntities.empty() == true)
			{
				ImGui::End();
				return;
			}

			ECS::Entity& selectedEntity = aEntities[aSelected];
			const std::string selectedEntityName = selectedEntity.GetName();

			char buffer[256];
			memset(buffer, '\0', sizeof(buffer));
			strncpy_s(buffer, selectedEntityName.c_str(), sizeof(buffer));
			buffer[sizeof(buffer) - 1] = '\0';

			ImGui::PushItemWidth(200);

			if (ImGui::InputTextWithHint("Name", "Entity Name", buffer, sizeof(buffer)))
			{
				if (MainSingleton::GetInputManager().IsKeyPressed(VK_RETURN))
				{
					const std::string newName(buffer);
					selectedEntity.SetName(newName);
				}
			}

			ImGui::PopItemWidth();

			ImGui::SameLine(ImGui::GetWindowWidth() - 70);
			ImGui::Text(std::string("ID: " + std::to_string(selectedEntity.GetID())).c_str());
			ImGui::Checkbox("Show Advanced", &myShowAdvanced);
			ImGui::Separator();

			if (aEntities.size() > 0)
			{
				ShowComponents(selectedEntity, aActiveECS);

				if (ImGui::Button("Add Component"))
				{
					ImGui::OpenPopup("Add Component");
				}

				if (ImGui::BeginPopup("Add Component"))
				{
					for (const auto& [hashCode, componentType] : MainSingleton::GetComponentRegistry()->myTypeErasureComponents)
					{
						const std::string componentNameLabel = componentType.myComponentPrettyName + "##Component";
						if (ImGui::Selectable(componentNameLabel.c_str()))
						{
							componentType.AddComponentFunctionPointer(selectedEntity);
						}
					}

					ImGui::EndPopup();
				}
			}
		}

		ImGui::End();
	}

	void EditorPopUp::ShowSceneHierachy(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected)
	{
		if (ImGui::Begin("Hierarchy"))
		{
			ShowAddPopUps(aActiveECS, aEntities, aSelected);
			ShowActiveSceneName();
			ShowSceneEntities(aEntities, aSelected);
		}

		ImGui::End();
	}

	void EditorPopUp::ShowComponents(ECS::Entity& aSelectedEntity, ECS::EntityComponentSystem& aActiveECS) const
	{
		const ECS::EntityID entityID = aSelectedEntity.GetID();
		const std::unordered_map<ECS::ComponentType, ECS::ComponentID>& componentMap = aSelectedEntity.GetComponentMap();

		for (const auto& [componentType, componentID] : componentMap)
		{
			ImGui::AlignTextToFramePadding();

			const size_t componentHashCode = componentType.hash_code();

			if (MainSingleton::GetComponentRegistry()->myTypeErasureComponents.contains(componentHashCode) == false)
			{
				continue;
			}

			const std::string& componentName = MainSingleton::GetComponentRegistry()->myTypeErasureComponents[componentHashCode].myComponentPrettyName + "##Component";
			const std::string componentIDAsText = "ID:" + std::to_string(componentID);
			void* componentPointer = aActiveECS.GetComponentPointerByComponentID(componentID);

			const bool isOpen = ImGui::TreeNodeEx(componentName.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);

			if (myShowAdvanced)
			{
				ImGui::SameLine();
				ImGui::Text(componentIDAsText.c_str());
			}

			ImGui::SameLine(ImGui::GetWindowWidth() - 50);

			if (ImGui::Button("..."))
			{
				ImGui::OpenPopup(std::string("ElementList" + std::to_string(entityID)).c_str());
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("More Options");
			}

			if (isOpen)
			{
				MainSingleton::GetComponentRegistry()->InspectComponentProperties(componentHashCode, componentPointer);
			}

			if (ImGui::BeginPopup(std::string("ElementList" + std::to_string(entityID)).c_str()))
			{
				if (ImGui::MenuItem("Remove Component"))
				{
					aSelectedEntity.RemoveComponentByTypeIndex(componentType);
					ImGui::EndPopup();

					if (isOpen)
					{
						ImGui::TreePop();
					}

					break;
				}

				ImGui::EndPopup();
			}

			if (isOpen)
			{
				ImGui::TreePop();
			}

			ImGui::Separator();
		}
	}

	void EditorPopUp::ShowSceneEntities(std::vector<ECS::Entity>& aEntities, int& aSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.18f, 0.18f, 0.18f, 0.80f).Value);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

		ImVec2 parentSize = ImGui::GetContentRegionAvail();

		if (ImGui::BeginListBox("##SceneEntities", parentSize))
		{
			for (int i = 0; i < aEntities.size(); ++i)
			{
				const bool isSelected = (aSelected == i);

				if (ImGui::Selectable(aEntities[i].GetName().c_str(), isSelected))
				{
					aSelected = i;
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{
						ImGui::OpenPopup("Entity Property##SceneHierachy");
					}

					if (ImGui::BeginPopup("Entity Property##SceneHierachy"))
					{
						if (ImGui::MenuItem("Remove##SceneHierachy"))
						{
							RemoveEntity(aEntities, aSelected);
							EditorEngine::mySelectedEntityID = static_cast<size_t>(-1);
							ImGui::EndPopup();
							break;
						}

						ImGui::EndPopup();
					}
				}
			}

			ImGui::EndListBox();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void EditorPopUp::ShowActiveSceneName()
	{
		ImGui::Separator();

		const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();

		char sceneName[256]{};
		memset(sceneName, '\0', sizeof(sceneName));
		strncpy_s(sceneName, sceneInfo->name.c_str(), sizeof(sceneName));
		sceneName[sizeof(sceneName) - 1] = '\0';

		if (ImGui::InputTextWithHint("Scene##SceneNameHierachy", "Name", sceneName, sizeof(sceneName)))
		{
			if (MainSingleton::GetInputManager().IsKeyPressed(VK_RETURN))
			{
				std::string newSceneName(sceneName);
				MainSingleton::GetSceneManager().ChangeSceneName(newSceneName);
			}
		}

		ImGui::Separator();
	}

	void EditorPopUp::RemoveEntity(std::vector<ECS::Entity>& aEntities, int& aSelected)
	{
		aEntities[aSelected].DestroyThis();

		if (aSelected >= static_cast<int>(aEntities.size() - 1))
		{
			aSelected--;
		}

		if (aSelected < 0 && aEntities.size() > 0)
		{
			aSelected = 0;
		}
	}

	void EditorPopUp::ShowAddPopUps(ECS::EntityComponentSystem& aActiveECS, std::vector<ECS::Entity>& aEntities, int& aSelected)
	{
		if (ImGui::Button("Add##SceneHierachy"))
		{
			ImGui::OpenPopup("Add Scene Object##SceneHierachy");
		}

		/*
		*
		* TO-DO(v11.0.6): implement name search filter which will require how ecs work i assume?
		*
		ImGui::SameLine(ImGui::GetWindowWidth() - 135);
		ImGui::PushItemWidth(125);

		std::string sceneSearch = "";
		if (ImGui::InputTextWithHint("##SearchScene", "Search", &sceneSearch[0], sceneSearch.capacity() + 1))
		{
		}

		ImGui::PopItemWidth();
		*/

		if (ImGui::BeginPopup("Add Scene Object##SceneHierachy"))
		{
			if (ImGui::MenuItem("Add Entity##SceneHierachy"))
			{
				ECS::Entity& entity = aActiveECS.CreateEntity();
				entity.AddComponent<ECS::TransformComponent>();
				aSelected = static_cast<int>(aEntities.size()) - 1;
			}

			if (ImGui::MenuItem("Add Cube##SceneHierachy"))
			{
				ECS::Entity& entity = aActiveECS.CreateEntity();

				entity.AddComponent<ECS::TransformComponent>();
				entity.AddComponent<ECS::MeshComponent>();

				aSelected = static_cast<int>(aEntities.size()) - 1;
			}

			ImGui::EndPopup();
		}
	}
}
