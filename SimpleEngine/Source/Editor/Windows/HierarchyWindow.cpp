#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/HierarchyWindow.hpp"
#include "Editor/FileManager/FileManager.hpp"
#include "Engine/ECS/Components/AllEngineComponents.hpp"
#include "Engine/ECS/ECSReflection.hpp"

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
		ECS::Entities entities = World::GetECS()->GetAllEntities();

		if (ImGui::Begin("Hierarchy"))
		{
			ImVec2 windowSize = ImGui::GetWindowSize();

			if (ImGui::Button("Add"))
			{
				ImGui::OpenPopup("Add Scene Object");
			}

			ImGui::SameLine(ImGui::GetWindowWidth() - 135);
			ImGui::PushItemWidth(125);

			std::string sceneSearch = "";

			if (ImGui::InputTextWithHint("##SearchScene", "Search", &sceneSearch[0], sceneSearch.capacity() + 1))
			{
			}

			ImGui::PopItemWidth();

			if (ImGui::BeginPopup("Add Scene Object"))
			{
				if (ImGui::MenuItem("Add Entity"))
				{
					World::GetECS()->CreateEntity();
					selected = static_cast<int>(entities.GetEntityCount()) - 1;
				}

				//if (ImGui::MenuItem("Add Cube"))
				//{
				//	ECS::Entity entity = World::GetECS()->CreateEntity();

				//	AddComponent(entity, eComponentType::Transform);
				//	AddComponent(entity, eComponentType::Mesh);

				//	selected = static_cast<int>(entities.GetEntityCount()) - 1;
				//}

				ImGui::EndPopup();
			}

			ImGui::Separator();
			ImGui::Text(World::GetActiveScene()->GetSceneName().c_str());
			ImGui::Separator();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.18f, 0.18f, 0.18f, 0.80f).Value);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

			ImVec2 parentSize = ImGui::GetContentRegionAvail();

			if (ImGui::BeginListBox("##SceneEntities", parentSize))
			{
				for (int i = 0; i < entities.GetEntityCount(); ++i)
				{
					const bool isSelected = (selected == i);

					if (ImGui::Selectable(entities[i]->GetName().c_str(), isSelected))
					{
						selected = i;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}

				}

				ImGui::EndListBox();
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}

		ImGui::End();

		if (selected < 0)
		{
			return;
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

					if (ComponentRegistry::myTypeErasureComponents.contains(componentHashCode) == false)
					{
						continue;
					}

					const std::string& componentName = ComponentRegistry::myTypeErasureComponents[componentHashCode].myComponentName;
					void* componentPointer = World::GetECS()->myComponentManager.GetComponentByComponentID(componentID);

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
						ComponentRegistry::InspectComponentProperties(componentHashCode, componentPointer);
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
					for (const auto& [hashCode, componentType] : ComponentRegistry::myTypeErasureComponents)
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
}