#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/EditorPopUp.hpp"
#include "Editor/Editor.hpp"
#include "Engine/ECS/Components/AllEngineComponents.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "Editor/Command/Commands/SelectEntityCommand.hpp"

namespace Editor
{
	EditorPopUp::EditorPopUp(const std::string& aName, CommandTracker* aCommandTracker)
		: PopUp(aName)
		, myShowAdvanced(false)
		, myCommandTracker(aCommandTracker)
	{
	}

	void EditorPopUp::Init()
	{
		myTemporaryECSEditor = std::make_unique<ECS::EntityComponentSystem>();
		myTemporaryECSEditor->Init();

		ECS::EntityComponentSystem& activeECS = MainSingleton::GetSceneManager().GetCurrentECS();
		std::vector<ECS::Entity>& entities = activeECS.GetAllEntities();

		myVisibleEntityIDs.resize(entities.size());

		for (size_t i = 0; i < myVisibleEntityIDs.size(); i++)
		{
			myVisibleEntityIDs[i] = entities[i].GetID();
		}
	}

	template<std::integral T>
	constexpr T GetInvalidIndex()
	{
		return std::numeric_limits<T>::max();
	}

	void EditorPopUp::Render()
	{
		ECS::EntityComponentSystem& activeECS = MainSingleton::GetSceneManager().GetCurrentECS();

		ShowSceneHierachy(activeECS);

		if (mySelectedEntityIndex < 0)
		{
			mySelectedEntityIndex = 0;
		}
		else if (mySelectedEntityIndex >= static_cast<int>(myVisibleEntityIDs.size()))
		{
			mySelectedEntityIndex = static_cast<int>(myVisibleEntityIDs.size() - 1);
		}

		if (myVisibleEntityIDs.empty() == false)
		{
			EditorEngine::mySelectedEntityID = myVisibleEntityIDs[mySelectedEntityIndex];
		}
		else
		{
			EditorEngine::mySelectedEntityID = GetInvalidIndex<ECS::EntityID>();
		}

		ShowInspector(activeECS);

		if (MainSingleton::GetInputManager().IsKeyPressed(VK_DELETE))
		{
			if (!myVisibleEntityIDs.empty())
			{
				if (mySelectedEntityIndex >= 0)
				{
					RemoveSelectedEntity();
					EditorEngine::mySelectedEntityID = GetInvalidIndex<ECS::EntityID>();
					return;
				}
			}
		}

		if (MainSingleton::GetInputManager().IsKeyPressed('K'))
		{
			if (!myVisibleEntityIDs.empty())
			{
				if (mySelectedEntityIndex >= 0)
				{
					ECS::EntityID createdEntityID = GetSelectedEntity()->DuplicateThis();
					myVisibleEntityIDs.insert(begin(myVisibleEntityIDs) + mySelectedEntityIndex + 1, createdEntityID);
				}
			}
		}

		static ECS::EntityID tempEntityID = GetInvalidIndex<ECS::EntityID>();

		if (MainSingleton::GetInputManager().IsKeyHeld(VK_CONTROL) && MainSingleton::GetInputManager().IsKeyPressed('C'))
		{
			if (!myVisibleEntityIDs.empty())
			{
				if (mySelectedEntityIndex >= 0)
				{
					tempEntityID = GetSelectedEntity()->DuplicateThisToAnotherECS(myTemporaryECSEditor->GetEntityManager());
				}
			}
		}
		else if (MainSingleton::GetInputManager().IsKeyHeld(VK_CONTROL) && MainSingleton::GetInputManager().IsKeyPressed('V'))
		{
			if (!myVisibleEntityIDs.empty())
			{
				if (mySelectedEntityIndex >= 0)
				{
					if (tempEntityID != GetInvalidIndex<ECS::EntityID>())
					{
						ECS::Entity& tempEntity = myTemporaryECSEditor->GetEntity(tempEntityID);
						ECS::EntityID createdEntityID = tempEntity.DuplicateThisToAnotherECS(activeECS.GetEntityManager());

						myVisibleEntityIDs.push_back(createdEntityID);

						SelectEntity(myVisibleEntityIDs.size());
					}
				}
			}
		}

		RemoveMeLater();
		RemoveMeLaterLater();
	}

	void EditorPopUp::RemoveMeLater()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 3));

		if (ImGui::Begin("Game##MainMenuBar", 0, ImGuiWindowFlags_NoScrollbar))
		{
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
			Graphics::RenderTargetManager* const renderTargetManager = graphicsEngine->GetRenderTargetManager();
			const Graphics::eRasterizerState currentRasterizerState = graphicsEngine->GetStateManager()->GetCurrentRasterizerState();

			ImTextureID textureID = renderTargetManager->GetShaderResourceView(Graphics::eRenderTargetType::PostProcessing).Get();

			if (currentRasterizerState != Graphics::eRasterizerState::BackfaceCulling
				&& currentRasterizerState != Graphics::eRasterizerState::NoFaceCulling)
			{
				textureID = renderTargetManager->GetShaderResourceView(Graphics::eRenderTargetType::Deferred).Get();
			}

			const ImVec2 size = ImGui::GetContentRegionAvail();
			ImGui::Image(textureID, size);

			ShowEntityTransformGizmo();
		}
		ImGui::End();

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

	}

	void EditorPopUp::RemoveMeLaterLater()
	{
		if (ImGui::BeginMainMenuBar())
		{
			const float distanceFromStart = ImGui::GetWindowWidth() - ImGui::GetContentRegionAvail().x;

			ImGui::Dummy(ImVec2(-distanceFromStart + ImGui::GetWindowWidth() * 0.5f - 38.0f, 0));

			Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

			const bool isPlaying = sceneManager.GetIsPlaying();

			if (isPlaying == true)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImColor(1.0f, 0.0f, 0.0f, 1.0f).Value);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0.6f, 0.0f, 0.0f, 1.0f).Value);
				Simple::ImGuiEngine::SetEditorMode(Simple::eImGuiEditorMode::Playing);

				ECS::EntityComponentSystem& ecs = sceneManager.GetCurrentECS();
				const std::unordered_set<ECS::EntityID>& cameraEntityIDs = ecs.GetEntityIDsWithThisComponent<ECS::CameraComponent>();

				if (cameraEntityIDs.empty() == false)
				{
					ECS::CameraComponent* cameraComponent = ecs.GetEntity(*cameraEntityIDs.begin()).GetComponent<ECS::CameraComponent>();

					if (cameraComponent != nullptr)
					{
						graphicsEngine->SetCamera(&cameraComponent->camera);
					}
				}
			}
			else
			{
				Simple::ImGuiEngine::SetEditorMode(Simple::eImGuiEditorMode::Default);
				graphicsEngine->SetCamera(graphicsEngine->GetEditorCamera().get());
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
			if (ImGui::Button(ICON_FA_PLAY))
			{
				sceneManager.SetIsPlaying(!isPlaying);
			}
			ImGui::PopStyleVar();

			if (isPlaying == true)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}

			ImGui::EndMainMenuBar();
		}

		{	//Render Orientation Cube  
			//TO-DO(v11.4.4): Make own class for this
			const Graphics::Camera* camera = Global::GetGraphicsEngine()->GetCurrentCamera();
			Math::Matrix4x4f view = camera->GetViewMatrix();
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImGuizmo::ViewManipulate(&view(1, 1), 16, ImVec2(windowPos.x + 775, windowPos.y + 375), ImVec2(64, 64), 0x00000000);
		}
	}

	ECS::Entity* EditorPopUp::GetSelectedEntity() const
	{
		if (mySelectedEntityIndex == GetInvalidIndex<size_t>())
		{
			return nullptr;
		}
		return &MainSingleton::GetSceneManager().GetCurrentECS().GetEntity(myVisibleEntityIDs[mySelectedEntityIndex]);
	}

	void EditorPopUp::ShowEntityTransformGizmo()
	{
		if (EditorEngine::mySelectedEntityID == GetInvalidIndex<ECS::EntityID>()) //TO-DO(v11.4.1): This shouldn't be here pls fix, future me
		{
			return;
		}

		const ImVec2 topLeft = ImGui::GetItemRectMin();
		const ImVec2 bottomRight = ImGui::GetItemRectMax();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		ImGuizmo::SetRect(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);

		ECS::EntityID selectedEntityID = EditorEngine::mySelectedEntityID;
		ECS::Entity& selectedEntity = MainSingleton::GetSceneManager().GetCurrentECS().GetEntity(selectedEntityID);
		ECS::TransformComponent* transformComponent = selectedEntity.GetComponent<ECS::TransformComponent>();

		if (transformComponent == nullptr)
		{
			return;
		}
		const Graphics::Camera* camera = Global::GetGraphicsEngine()->GetCurrentCamera();

		Math::Matrix4x4f objectMatrix = transformComponent->transform.GetMatrix();
		const Math::Matrix4x4f view = camera->GetViewMatrix();
		const Math::Matrix4x4f proj = camera->GetProjectionMatrix();

		static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;

		const Simpleton::InputManager& inputManager = MainSingleton::GetInputManager();
		if (!inputManager.GetMouseIsHidden() && !inputManager.IsKeyHeld(VK_CONTROL))
		{
			if (inputManager.IsKeyPressed('T'))
			{
				operation = ImGuizmo::OPERATION::TRANSLATE;
			}
			else if (inputManager.IsKeyPressed('R'))
			{
				operation = ImGuizmo::OPERATION::ROTATE;
			}
			else if (inputManager.IsKeyPressed('S'))
			{
				operation = ImGuizmo::OPERATION::SCALE;
			}
		}

		const bool isManipulatingEntityTransform = ImGuizmo::Manipulate(&view(1, 1),
			&proj(1, 1),
			operation,
			ImGuizmo::MODE::WORLD,
			&objectMatrix(1, 1)
		);

		if (isManipulatingEntityTransform && inputManager.IsKeyDown(VK_LBUTTON) && !myIsDraggingEntity)
		{
			myIsDraggingEntity = true;
			mySetEntityTransformCommand.myEntityID = selectedEntityID;
			mySetEntityTransformCommand.myOldTransform = transformComponent->transform;
		}

		if (isManipulatingEntityTransform)
		{
			switch (operation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				transformComponent->transform.SetPosition(objectMatrix.GetPosition());
				break;
			case ImGuizmo::OPERATION::ROTATE:
				transformComponent->transform.SetMatrix(objectMatrix);
				break;
			case ImGuizmo::OPERATION::SCALE:
				transformComponent->transform.SetScale(objectMatrix.GetScale());
				break;
			default:
				break;
			}
		}


		if (inputManager.IsKeyReleased(VK_LBUTTON) && myIsDraggingEntity && selectedEntityID == mySetEntityTransformCommand.myEntityID)
		{
			myIsDraggingEntity = false;
			mySetEntityTransformCommand.myNewTransform = transformComponent->transform;
			myCommandTracker->RegisterCommand(Command(mySetEntityTransformCommand, "Set Entity Transform"));
		}

	}

	void EditorPopUp::ShowInspector(ECS::EntityComponentSystem& aActiveECS)
	{
		if (ImGui::Begin("Inspector##HierachyWindow"))
		{
			if (myVisibleEntityIDs.empty() == true)
			{
				ImGui::End();
				return;
			}

			ECS::EntityID selectedEntityID = myVisibleEntityIDs[mySelectedEntityIndex];
			ECS::Entity& selectedEntity = aActiveECS.GetEntity(selectedEntityID);
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

			if (myVisibleEntityIDs.size() > 0)
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

	void EditorPopUp::ShowSceneHierachy(ECS::EntityComponentSystem& aActiveECS)
	{
		if (ImGui::Begin("Hierarchy"))
		{
			ShowAddPopUps(aActiveECS);
			ShowActiveSceneName();
			ShowSceneEntities();
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

	void EditorPopUp::ShowSceneEntities()
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.18f, 0.18f, 0.18f, 0.80f).Value);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

		ImVec2 parentSize = ImGui::GetContentRegionAvail();

		if (ImGui::BeginListBox("##SceneEntities", parentSize))
		{
			for (int i = 0; i < myVisibleEntityIDs.size(); ++i)
			{
				ECS::Entity& entity = MainSingleton::GetSceneManager().GetCurrentECS().GetEntity(myVisibleEntityIDs[i]);
				const bool isSelected = (mySelectedEntityIndex == i);

				if (ImGui::Selectable(entity.GetName().c_str(), isSelected))
				{
					SelectEntity(i);
				}

				if (isSelected == false)
				{
					continue;
				}

				ImGui::SetItemDefaultFocus();

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("Entity Property##SceneHierachy");
				}

				if (ImGui::BeginPopup("Entity Property##SceneHierachy"))
				{
					if (ImGui::MenuItem("Remove##SceneHierachy"))
					{
						RemoveEntity(mySelectedEntityIndex);

						EditorEngine::mySelectedEntityID = GetInvalidIndex<ECS::EntityID>();
					}

					ImGui::EndPopup();
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

	void EditorPopUp::RemoveSelectedEntity()
	{
		RemoveEntity(mySelectedEntityIndex);
	}

	void EditorPopUp::RemoveEntity(size_t aEntityIndex)
	{

		class RemoveEntityCommand final
		{
		public:
			size_t myHierarchyIndex = GetInvalidIndex<size_t>();
			ECS::EntityID myEntityID = GetInvalidIndex<ECS::EntityID>();
			std::vector<ECS::EntityID>* myEntityIDs = nullptr;

			void Execute() const
			{
				myEntityIDs->erase(begin(*myEntityIDs) + myHierarchyIndex);
			}

			void Undo() const
			{
				myEntityIDs->insert(begin(*myEntityIDs) + myHierarchyIndex, myEntityID);
			}
		};

		RemoveEntityCommand removeEntityCommand;
		removeEntityCommand.myHierarchyIndex = aEntityIndex;
		removeEntityCommand.myEntityIDs = &myVisibleEntityIDs;
		removeEntityCommand.myEntityID = myVisibleEntityIDs[aEntityIndex];

		myCommandTracker->BeginComposite("Remove Entity Composite");

		myCommandTracker->ExecuteCommand(Command(removeEntityCommand, "Remove Entity"));

		size_t newSelectedIndex = mySelectedEntityIndex;
		if (mySelectedEntityIndex >= myVisibleEntityIDs.size() - 1)
		{
			newSelectedIndex = mySelectedEntityIndex - 1;
		}
		SelectEntity(newSelectedIndex);

		if (mySelectedEntityIndex < 0 && myVisibleEntityIDs.size() > 0)
		{
			mySelectedEntityIndex = 0;
		}

		myCommandTracker->EndComposite();
	}

	ECS::EntityID EditorPopUp::CreateEntity()
	{

		ECS::Entity& entity = MainSingleton::GetSceneManager().GetCurrentECS().CreateEntity();

		class CreateEntityCommand final
		{
		public:
			size_t myHierarchyIndex = GetInvalidIndex<size_t>();
			ECS::EntityID myEntityID = GetInvalidIndex<ECS::EntityID>();
			std::vector<ECS::EntityID>* myEntityIDs = nullptr;

			void Execute() const
			{
				myEntityIDs->push_back(myEntityID);
			}

			void Undo() const
			{
				myEntityIDs->erase(begin(*myEntityIDs) + myHierarchyIndex);
			}
		};

		CreateEntityCommand createEntityCommand;
		createEntityCommand.myHierarchyIndex = myVisibleEntityIDs.size();
		createEntityCommand.myEntityIDs = &myVisibleEntityIDs;
		createEntityCommand.myEntityID = entity.GetID();

		myCommandTracker->ExecuteCommand(Command(createEntityCommand, "Create Entity"));

		return entity.GetID();
	}

	void EditorPopUp::ShowAddPopUps(ECS::EntityComponentSystem& aActiveECS)
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
				myCommandTracker->BeginComposite("Create Entity Composite");
				ECS::EntityID createdEntityID = CreateEntity();
				ECS::Entity& entity = aActiveECS.GetEntity(createdEntityID);

				entity.AddComponent<ECS::TransformComponent>();

				SelectEntity(static_cast<int>(myVisibleEntityIDs.size()));

				myCommandTracker->EndComposite();
			}

			if (ImGui::MenuItem("Add Cube##SceneHierachy"))
			{
				myCommandTracker->BeginComposite("Create Entity Composite");

				ECS::EntityID createdEntityID = CreateEntity();
				ECS::Entity& entity = aActiveECS.GetEntity(createdEntityID);

				entity.AddComponent<ECS::TransformComponent>();
				entity.AddComponent<ECS::MeshComponent>();

				SelectEntity(static_cast<int>(myVisibleEntityIDs.size()));

				myCommandTracker->EndComposite();
			}

			ImGui::EndPopup();
		}
	}

	void EditorPopUp::SelectEntity(size_t aNewIndex)
	{
		if (aNewIndex == mySelectedEntityIndex)
		{
			return;
		}
		SelectEntityCommand selectEntityCommand;
		selectEntityCommand.myNewIndex = aNewIndex;
		selectEntityCommand.myOldIndex = mySelectedEntityIndex;
		selectEntityCommand.mySelectedEntityIndexPtr = &mySelectedEntityIndex;

		myCommandTracker->ExecuteCommand(Command(selectEntityCommand, "Select Entity"));
	}
}
