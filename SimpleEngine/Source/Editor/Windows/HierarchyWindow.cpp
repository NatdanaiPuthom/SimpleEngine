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

			ImGui::SameLine(ImGui::GetWindowWidth() - 70);
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

						if (componentNames[i] == "TransformComponent") //NOTE(v9.34.0): Ugly hardcoded, trying to fix reflection
						{
							ECS::TransformComponent* transformComponent = selectedEntity->GetComponent<ECS::TransformComponent>();
							Math::Vector3f position = transformComponent->transform.GetPosition();
							Math::Vector3f rotation = transformComponent->transform.GetRotation();
							Math::Vector3f scale = transformComponent->transform.GetScale();

							ImGui::SetNextItemWidth(200);

							if (ImGui::DragFloat3("Position", &position.x, 0.1f))
							{
								transformComponent->transform.SetPosition(position);
							}

							ImGui::SetNextItemWidth(200);

							if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f))
							{
								transformComponent->transform.SetRotation(rotation);
							}

							ImGui::SetNextItemWidth(200);

							if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
							{
								transformComponent->transform.SetScale(scale);
							}
						}
						else if (componentNames[i] == "MeshComponent")
						{
							ECS::MeshComponent* meshComponent = selectedEntity->GetComponent<ECS::MeshComponent>();

							const std::string textureName = "Texture: " + meshComponent->texture->GetShaderName();
							const std::string meshName = "Mesh: " + meshComponent->mesh->GetMeshName();

							ImGui::Text(textureName.c_str());
							ImGui::Text(meshName.c_str());


							//static char droppedFilePath[256] = "";

							//ImGui::Text("Drag and drop a file into this box:");
							//ImGui::BeginChild("DroppableBox", ImVec2(0, 100), true);
							//ImVec2 boxSize = ImGui::GetContentRegionAvail();
							//ImGui::Text("Drop file here");
							//ImGui::EndChild();

							////ImGui::SetDragDropPayload("FILE_PATH", filePath.c_str(), filePath.size() + 1);

							//if (ImGui::BeginDragDropTarget())
							//{
							//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
							//	{
							//		// Handle the dropped file
							//		const char* filePath = (const char*)payload->Data;
							//		strncpy_s(droppedFilePath, filePath, sizeof(droppedFilePath));
							//		droppedFilePath[sizeof(droppedFilePath) - 1] = '\0'; // Ensure null-terminated
							//	}

							//	ImGui::EndDragDropTarget();
							//}

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
					std::array<std::string, 5> components;
					components[0] = "TransformComponent";
					components[1] = "MeshComponent";
					components[2] = "AnimatedComponent";
					components[3] = "AnimationPlayerComponent";
					components[4] = "NullComponent";

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
								selectedEntity->AddComponent<ECS::MeshComponent>();
								break;
							case 2:
								selectedEntity->AddComponent<ECS::AnimatedComponent>();
								break;
							case 3:
								selectedEntity->AddComponent<ECS::AnimationPlayerComponent>();
								break;
							case 4:
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

		if (SimpleUtilities::InputManager::GetInstance().IsKeyPressed(VK_DELETE))
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
}