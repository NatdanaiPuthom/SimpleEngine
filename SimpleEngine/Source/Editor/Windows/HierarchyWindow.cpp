#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/HierarchyWindow.hpp"
#include "Editor/FileManager/FileManager.hpp"
#include "Engine/ECS/Components/AllEngineComponents.hpp"

namespace Editor
{
	enum class eComponentType : size_t
	{
		Transform,
		Mesh,
		Animated,
		AnimationPlayer,
		Null,
		Count
	};

	static void AddComponent(ECS::Entity aEntity, const eComponentType aType)
	{
		switch (aType)
		{
		case eComponentType::Transform:
			aEntity->AddComponent<ECS::TransformComponent>();
			break;
		case eComponentType::Mesh:
			aEntity->AddComponent<ECS::MeshComponent>();
			aEntity->GetComponent<ECS::MeshComponent>()->shader = Global::GetGraphicsEngine()->GetShader(Graphics::eShaderType::Unlit_Default).get();
			break;
		case eComponentType::Animated:
			aEntity->AddComponent<ECS::AnimatedComponent>();
			break;
		case eComponentType::AnimationPlayer:
			aEntity->AddComponent<ECS::AnimationPlayerComponent>();
			break;
		case eComponentType::Null:
			aEntity->AddComponent<ECS::NullComponent>();
			break;
		}
	}

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

				if (ImGui::MenuItem("Add Cube"))
				{
					ECS::Entity entity = World::GetECS()->CreateEntity();

					AddComponent(entity, eComponentType::Transform);
					AddComponent(entity, eComponentType::Mesh);

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

							std::string albedoName = "Albedo: ";
							std::string materialName = "Material: ";
							std::string normalName = "Normal: ";

							std::string meshName = "Mesh: ";
							std::string pixelShaderName = "Pixel Shader: ";
							std::string vertexShaderName = "Vertex Shader: ";

							if (meshComponent->mesh != nullptr)
							{
								meshName += meshComponent->mesh->GetMeshName();
							}

							if (meshComponent->textures[Graphics::Global_Slot_Albedo] != nullptr)
							{
								albedoName += meshComponent->textures[Graphics::Global_Slot_Albedo]->GetShaderName();
							}

							if (meshComponent->textures[Graphics::Global_Slot_Material] != nullptr)
							{
								materialName += meshComponent->textures[Graphics::Global_Slot_Material]->GetShaderName();
							}

							if (meshComponent->textures[Graphics::Global_Slot_Normal] != nullptr)
							{
								normalName += meshComponent->textures[Graphics::Global_Slot_Normal]->GetShaderName();
							}

							if (meshComponent->shader != nullptr)
							{
								pixelShaderName += meshComponent->shader->GetPixelShaderName();
								vertexShaderName += meshComponent->shader->GetVertexShaderName();
							}

							ImGui::Text(albedoName.c_str());

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
								{
									const std::string payloadData = reinterpret_cast<const char*>(payload->Data);
									const std::string extension = FileManager::GetFileExtension(payloadData);

									if (extension == ".dds")
									{
										const std::string fileName = SimpleUtilities::KeepStringAfterAssets(payloadData);
										meshComponent->textures[Graphics::Global_Slot_Albedo] = Global::GetGraphicsEngine()->GetTexture(fileName.c_str()).get();
									}
								}

								ImGui::EndDragDropTarget();
							}

							ImGui::Text(materialName.c_str());

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
								{
									const std::string payloadData = reinterpret_cast<const char*>(payload->Data);
									const std::string extension = FileManager::GetFileExtension(payloadData);

									if (extension == ".dds")
									{
										const std::string fileName = SimpleUtilities::KeepStringAfterAssets(payloadData);
										meshComponent->textures[Graphics::Global_Slot_Material] = Global::GetGraphicsEngine()->GetTexture(fileName.c_str()).get();
									}
								}

								ImGui::EndDragDropTarget();
							}

							ImGui::Text(normalName.c_str());

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
								{
									const std::string payloadData = reinterpret_cast<const char*>(payload->Data);
									const std::string extension = FileManager::GetFileExtension(payloadData);

									if (extension == ".dds")
									{
										const std::string fileName = SimpleUtilities::KeepStringAfterAssets(payloadData);
										meshComponent->textures[Graphics::Global_Slot_Normal] = Global::GetGraphicsEngine()->GetTexture(fileName.c_str()).get();
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

							ImGui::Text(pixelShaderName.c_str());
							ImGui::Text(vertexShaderName.c_str());
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
					std::array<std::string, static_cast<size_t>(eComponentType::Count)> components;
					components[static_cast<size_t>(eComponentType::Transform)] = "TransformComponent";
					components[static_cast<size_t>(eComponentType::Mesh)] = "MeshComponent";
					components[static_cast<size_t>(eComponentType::Animated)] = "AnimatedComponent";
					components[static_cast<size_t>(eComponentType::AnimationPlayer)] = "AnimationPlayerComponent";
					components[static_cast<size_t>(eComponentType::Null)] = "NullComponent";

					for (size_t i = 0; i < components.size(); ++i)
					{
						if (ImGui::Selectable(components[i].c_str()))
						{
							AddComponent(selectedEntity, static_cast<eComponentType>(i));
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