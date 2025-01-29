#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/EditorPopUp.hpp"
#include "Editor/EditorEngine.hpp"
#include "Engine/ECS/Components/AllEngineComponents.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "Editor/Command/Commands/SelectEntityCommand.hpp"
#include "Engine/Memory/DynamicMemoryArena.hpp"
#include "Editor/Utility/EditorUtilities.hpp"

namespace Editor
{
	EditorPopUp::EditorPopUp(const std::string& aName, CommandTracker* aCommandTracker)
		: PopUp(aName)
		, myShowAdvanced(false)
		, myCommandTracker(aCommandTracker)
		, myGridSnapTool(&myTransformEntityTool)
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
	}

	ECS::Entity* EditorPopUp::GetSelectedEntity() const
	{
		if (mySelectedEntityIndex == GetInvalidIndex<size_t>())
		{
			return nullptr;
		}
		return &MainSingleton::GetSceneManager().GetCurrentECS().GetEntity(myVisibleEntityIDs[mySelectedEntityIndex]);
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

		static bool previousFrameEditingAnyActive = false;
		static Simple::DynamicMemoryArenaHandle copiedComponentMemoryHandle;
		static ECS::ComponentID editedComponentID = GetInvalidIndex<ECS::ComponentID>();
		static ECS::ComponentHashCode editedComponentHashCode = GetInvalidIndex<ECS::ComponentHashCode>();
		bool anyComponentsActiveCurrentFrame = false;

		Simple::DynamicMemoryArena componentFrameBufferArena(1000);

		static Simple::DynamicMemoryArena componentBufferArena(10000);

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
				const ECS::ComponentRegistry& componentRegistry = *MainSingleton::GetComponentRegistry();

				const size_t componentSize = componentRegistry.GetComponentSize(componentHashCode);
				ECS::InPlaceAllocateFunction inplaceAllocateFunction = componentRegistry.GetInplaceAllocateFunction(componentHashCode);
				ECS::DestroyFunction destroyFunction = componentRegistry.GetDestroyFunction(componentHashCode);
				ECS::CopyFunction copyFunction = componentRegistry.GetCopyFunction(componentHashCode);

				Simple::DynamicMemoryArenaHandle temporaryComponentHandle = componentFrameBufferArena.AllocateUnsafe(componentSize,
					inplaceAllocateFunction, destroyFunction, copyFunction);

				componentRegistry.CopyComponent(componentHashCode, componentFrameBufferArena.MemoryAt(temporaryComponentHandle), componentPointer);

				const ECS::ViewAndEditResult viewAndEditResult = componentRegistry.InspectComponentProperties(componentHashCode, componentPointer);

				anyComponentsActiveCurrentFrame |= viewAndEditResult.myIsActive;
				if (viewAndEditResult.myIsActive && !previousFrameEditingAnyActive)
				{
					copiedComponentMemoryHandle = componentBufferArena.AllocateUnsafe(componentSize,
						inplaceAllocateFunction, destroyFunction, copyFunction);

					std::byte* componentPtr1 = componentFrameBufferArena.MemoryAt(temporaryComponentHandle);
					std::byte* componentPtr2 = componentBufferArena.MemoryAt(copiedComponentMemoryHandle);
					componentRegistry.SwapComponent(componentHashCode, componentPtr1, componentPtr2);

					editedComponentID = componentID;
					editedComponentHashCode = componentHashCode;
				}

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

		if (previousFrameEditingAnyActive && !anyComponentsActiveCurrentFrame)
		{

			class SetComponentValueCommand final
			{
			public:

				Simple::DynamicMemoryArenaHandle myMemoryHandle;
				Simple::DynamicMemoryArena* myMemoryArena = nullptr;
				ECS::ComponentHashCode myComponentHashCode = GetInvalidIndex<size_t>();
				ECS::ComponentID myComponentID = GetInvalidIndex<size_t>();

				void Execute() const
				{
					SwapDataPtrs();
				}

				void Undo() const
				{
					SwapDataPtrs();
				}

			private:

				void SwapDataPtrs() const
				{
					const ECS::ComponentRegistry& componentRegistry = *MainSingleton::GetComponentRegistry();

					void* dataPtr1 = MainSingleton::GetSceneManager().GetCurrentECS().GetComponentPointerByComponentID(myComponentID);
					std::byte* dataPtr2 = myMemoryArena->MemoryAt(myMemoryHandle);
					componentRegistry.SwapComponent(myComponentHashCode, dataPtr1, dataPtr2);
				}
			};

			SetComponentValueCommand setComponentValueCommand;
			setComponentValueCommand.myMemoryArena = &componentBufferArena;
			setComponentValueCommand.myMemoryHandle = copiedComponentMemoryHandle;
			setComponentValueCommand.myComponentHashCode = editedComponentHashCode;
			setComponentValueCommand.myComponentID = editedComponentID;

			myCommandTracker->RegisterCommand(Command(setComponentValueCommand, "Set Component Value"));
		}

		previousFrameEditingAnyActive = anyComponentsActiveCurrentFrame;
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

		ECS::EntityID entityID = MainSingleton::GetSceneManager().GetCurrentECS().CreateEntity();

		CreateEntityCommand createEntityCommand;
		createEntityCommand.myHierarchyIndex = myVisibleEntityIDs.size();
		createEntityCommand.myEntityIDs = &myVisibleEntityIDs;
		createEntityCommand.myEntityID = entityID;

		myCommandTracker->ExecuteCommand(Command(createEntityCommand, "Create Entity"));

		return entityID;
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
