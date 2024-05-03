#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/HierarchyWindow.hpp"
#include "Engine/Components/AllEngineComponents.hpp"

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
	}

	void HierarchyWindow::Draw()
	{
		static int selected = 0;
		ECS::Entities entities = World::GetECS()->GetAllEntities();

		if (ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_NoResize))
		{
			if (ImGui::Button("Add"))
			{
				ImGui::OpenPopup("Add Scene Object");
			}

			ImGui::SameLine(ImGui::GetWindowWidth() - 175);
			ImGui::PushItemWidth(150);
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

				ImGui::EndPopup();
			}

			ImGui::Separator();
			ImGui::Text(World::GetActiveScene()->GetSceneName().c_str());
			ImGui::Separator();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.18f, 0.18f, 0.18f, 0.80f).Value);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

			if (ImGui::BeginChild("SceneEntities", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Border))
			{
				if (ImGui::BeginListBox("##Entities"))
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

				ImGui::EndChild();
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

			std::string searchComponent = selectedEntity->GetName();
			ImGui::PushItemWidth(200);
			if (ImGui::InputTextWithHint("Name", "Entity Name", &searchComponent[0], searchComponent.capacity() + 1))
			{
				if (SimpleUtilities::InputManager::GetInstance().IsKeyPressed(VK_RETURN))
				{
					selectedEntity->SetName(searchComponent);
				}
			}
			ImGui::PopItemWidth();

			ImGui::SameLine(ImGui::GetWindowWidth() - 50);
			ImGui::Text(std::string("ID: " + std::to_string(selectedEntity->GetID())).c_str());
			ImGui::Separator();

			if (entities.GetEntityCount() > 0)
			{
				const size_t id = selectedEntity->GetID();
				const std::vector<std::string> componentNames = selectedEntity->GetComponentNames();

				for (size_t i = 0; i < componentNames.size(); ++i)
				{
					ImGui::AlignTextToFramePadding();

					ImGui::PushID(std::string(componentNames[i] + " " + std::to_string(id)).c_str());
					const bool open = ImGui::TreeNodeEx(componentNames[i].c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);
					ImGui::PopID();

					if (open)
					{
						ImGui::SameLine(ImGui::GetWindowWidth() - 50);

						if (ImGui::Button("..."))
						{
							ImGui::OpenPopup(std::string("ElementList" + std::to_string(id)).c_str());
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("More Options");
						}

						if (ImGui::BeginPopup(std::string("ElementList" + std::to_string(id)).c_str()))
						{
							if (ImGui::MenuItem("Remove Component"))
							{
								selectedEntity->RemoveComponentByTypeName(componentNames[i]);
							}

							ImGui::EndPopup();
						}

						ImGui::TreePop();
						ImGui::Separator();
					}
				}

				if (ImGui::Button("Add Component"))
				{
					ImGui::OpenPopup("Add Component");
				}

				if (ImGui::BeginPopup("Add Component"))
				{
					std::array<std::string, 2> components;
					components[0] = "Transform";
					components[1] = "NullComponent";

					for (size_t i = 0; i < components.size(); ++i)
					{
						if (ImGui::Selectable(components[i].c_str()))
						{
							switch (i)
							{
							case 0:
								selectedEntity->AddComponent<ECS::TransformComponent>();
								break;
							case 1:
								selectedEntity->AddComponent<ECS::NullComponent>();
								break;
							}
						}
					}

					ImGui::EndPopup();
				}
			}
		}

		ImGui::End();
	}
}