#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/HierarchyWindow.hpp"
#include "Editor/FileManager/FileManager.hpp"
#include "Engine/ECS/Components/AllEngineComponents.hpp"

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
								if (scale.x < 0.001f)
								{
									scale.x = 0.001f;
								}

								if (scale.y < 0.001f)
								{
									scale.y = 0.001f;
								}

								if (scale.z < 0.001f)
								{
									scale.z = 0.001f;
								}
							
								transformComponent->transform.SetScale(scale);
							}
						}
						else if (componentNames[i] == "MeshComponent")
						{
							ECS::MeshComponent* meshComponent = selectedEntity->GetComponent<ECS::MeshComponent>();

							std::string textureName = "Texture: ";
							std::string meshName = "Mesh: ";

							if (meshComponent->mesh != nullptr)
							{
								meshName += meshComponent->mesh->GetMeshName();
							}

							if (meshComponent->texture != nullptr)
							{
								textureName += meshComponent->texture->GetShaderName();
							}

							ImGui::Text(textureName.c_str());

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
								{
									const std::string payloadData = reinterpret_cast<const char*>(payload->Data);
									const std::string extension = FileManager::GetFileExtension(payloadData);

									if (extension == ".dds")
									{
										const std::string fileName = FileManager::GetFileName(payloadData);
										meshComponent->texture = Global::GetGraphicsEngine()->GetTexture(fileName.c_str()).get();
									}
								}

								ImGui::EndDragDropTarget();
							}

							ImGui::Text(meshName.c_str());

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
								{
									const std::string payloadData = reinterpret_cast<const char*>(payload->Data);
									const std::string extension = FileManager::GetFileExtension(payloadData);

									if (extension == ".fbx")
									{
										meshComponent->mesh = Global::GetModelFactory()->LoadMesh(payloadData);
									}
								}

								ImGui::EndDragDropTarget();
							}
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
								selectedEntity->GetComponent<ECS::MeshComponent>()->shader = Global::GetGraphicsEngine()->GetDefaultShader().get();
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
}