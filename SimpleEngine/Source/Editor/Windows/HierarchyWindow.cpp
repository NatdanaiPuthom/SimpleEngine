#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/HierarchyWindow.hpp"
#include "Editor/FileManager/FileManager.hpp"
#include "Engine/ECS/Components/AllEngineComponents.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	HierarchyWindow::HierarchyWindow()
	{
	}

	void HierarchyWindow::Init()
	{
	}

	void HierarchyWindow::Update()
	{
		static int selected = 0;

		ECS::EntityComponentSystem& activeECS = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::Entities entities = activeECS.GetAllEntities();

		if (ImGui::Begin("Hierarchy"))
		{
			ShowAddPopUps(activeECS, entities, selected);
			ShowActiveSceneName();
			ShowSceneEntities(entities, selected);
		}

		ImGui::End();

		if (selected < 0)
		{
			return;
		}
		else if (selected >= static_cast<int>(entities.GetEntityCount()))
		{
			selected = static_cast<int>(entities.GetEntityCount() - 1);
		}

		if (ImGui::Begin("Inspector"))
		{
			ECS::Entity selectedEntity = entities[selected];

			if (selectedEntity == nullptr)
			{
				ImGui::End();
				return;
			}

			std::string selectedEntityName = selectedEntity->GetName();

			ImGui::PushItemWidth(200);

			if (ImGui::InputTextWithHint("Name", "Entity Name", &selectedEntityName[0], selectedEntityName.capacity() + 1))
			{
				if (MainSingleton::GetInputManager().IsKeyPressed(VK_RETURN))
				{
					selectedEntity->SetName(selectedEntityName);
				}
			}

			ImGui::PopItemWidth();

			ImGui::SameLine(ImGui::GetWindowWidth() - 70);
			ImGui::Text(std::string("ID: " + std::to_string(selectedEntity->GetID())).c_str());
			ImGui::Separator();

			if (entities.GetEntityCount() > 0)
			{
				const size_t id = selectedEntity->GetID(); id;

				const std::unordered_map<ECS::ComponentType, ECS::ComponentID>& componentMap = selectedEntity->GetComponentMap();

				for (const auto& [componentType, componentID] : componentMap)
				{
					ImGui::AlignTextToFramePadding();

					const size_t componentHashCode = componentType.hash_code();

					if (MainSingleton::GetComponentRegistry()->myTypeErasureComponents.contains(componentHashCode) == false)
					{
						continue;
					}

					const std::string& componentName = MainSingleton::GetComponentRegistry()->myTypeErasureComponents[componentHashCode].myComponentName;
					void* componentPointer = activeECS.GetComponentPointerByComponentID(componentID);

					const bool isOpen = ImGui::TreeNodeEx(componentName.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);

					ImGui::SameLine(ImGui::GetWindowWidth() - 50);

					if (ImGui::Button("..."))
					{
						ImGui::OpenPopup(std::string("ElementList" + std::to_string(id)).c_str());
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("More Options");
					}

					if (isOpen)
					{
						MainSingleton::GetComponentRegistry()->InspectComponentProperties(componentHashCode, componentPointer);
					}

					if (ImGui::BeginPopup(std::string("ElementList" + std::to_string(id)).c_str()))
					{
						if (ImGui::MenuItem("Remove Component"))
						{
							selectedEntity->RemoveComponentByTypeIndex(componentType);
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

				if (ImGui::Button("Add Component"))
				{
					ImGui::OpenPopup("Add Component");
				}

				if (ImGui::BeginPopup("Add Component"))
				{
					for (const auto& [hashCode, componentType] : MainSingleton::GetComponentRegistry()->myTypeErasureComponents)
					{
						if (ImGui::Selectable(componentType.myComponentName.c_str()))
						{
							componentType.AddComponentFunctionPointer(selectedEntity);
						}
					}

					ImGui::EndPopup();
				}
			}
		}

		ImGui::End();

		if (MainSingleton::GetInputManager().IsKeyPressed(VK_DELETE))
		{
			if (const size_t count = entities.GetEntityCount() > 0)
			{
				if (selected >= 0)
				{
					entities[selected]->DestroyThis();

					if (selected >= static_cast<int>(count - 1))
					{
						selected--;
					}

					if (selected < 0 && entities.GetEntityCount() > 0)
					{
						selected = 0;
					}

					return;
				}
			}
		}
	}

	void HierarchyWindow::Draw()
	{

	}

	void HierarchyWindow::ShowActiveSceneName()
	{
		ImGui::Separator();

		const std::string activeSceneName = SimpleUtilities::ConvertFilePathToPrettyName(MainSingleton::GetSceneManager().GetCurrentScenePath());
		std::string sceneNameInput = activeSceneName;

		if (ImGui::InputTextWithHint("##SceneNameHierachy", "Name", &sceneNameInput[0], sceneNameInput.capacity() + 1))
		{
			/*if (MainSingleton::GetInputManager().IsKeyPressed(VK_RETURN))
			{
				if (std::rename(SimpleUtilities::GetAbsolutePath(std::string(SIMPLE_DIR_SCENES) + "\\" + activeSceneName).c_str(), SimpleUtilities::GetAbsolutePath(std::string(SIMPLE_DIR_SCENES) + "\\" + sceneNameInput).c_str()) != 0)
				{
					assert(false && "Failed to rename the scene");
					return;
				}
			}*/
		}

		ImGui::Separator();
	}

	void HierarchyWindow::ShowSceneEntities(ECS::Entities& aEntities, int& aSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.18f, 0.18f, 0.18f, 0.80f).Value);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

		ImVec2 parentSize = ImGui::GetContentRegionAvail();

		if (ImGui::BeginListBox("##SceneEntities", parentSize))
		{
			for (int i = 0; i < aEntities.GetEntityCount(); ++i)
			{
				const bool isSelected = (aSelected == i);

				if (ImGui::Selectable(aEntities[i]->GetName().c_str(), isSelected))
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
							aEntities[aSelected]->DestroyThis();

							if (aSelected >= static_cast<int>(aEntities.GetEntityCount() - 1))
							{
								aSelected--;
							}

							if (aSelected < 0 && aEntities.GetEntityCount() > 0)
							{
								aSelected = 0;
							}

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

	void HierarchyWindow::ShowAddPopUps(ECS::EntityComponentSystem& aActiveECS, ECS::Entities& aEntities, int& aSelected)
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
				aActiveECS.CreateEntity();
				aSelected = static_cast<int>(aEntities.GetEntityCount()) - 1;
			}

			if (ImGui::MenuItem("Add Cube##SceneHierachy"))
			{
				ECS::Entity entity = aActiveECS.CreateEntity();

				entity->AddComponent<ECS::TransformComponent>();
				entity->AddComponent<ECS::MeshComponent>();

				aSelected = static_cast<int>(aEntities.GetEntityCount()) - 1;
			}

			ImGui::EndPopup();
		}
	}
}