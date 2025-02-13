#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/SceneInspectorPopUp.hpp"
#include "Editor/Utility/EditorUtilities.hpp"
#include "Editor/Command/Core/EditorCommandTracker.hpp"
#include "Engine/Memory/DynamicMemoryArena.hpp"
#include "Editor/EditorProxy.hpp"

namespace Editor
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
}

namespace Editor
{
	SceneInspectorPopUp::SceneInspectorPopUp(const std::string& aName)
		: PopUp(aName)
		, myShowAdvanced(false)
	{
	}

	void SceneInspectorPopUp::Render()
	{
		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			std::vector<size_t>& visibleEntityIDs = EditorProxy::GetVisibleEntityIDsRef();
			ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();

			if (visibleEntityIDs.empty() == true)
			{
				ImGui::End();
				return;
			}
			
			ECS::EntityID selectedEntityID = visibleEntityIDs[EditorProxy::GetSelectedEntityIndex()];
			ECS::Entity& selectedEntity = ecs.GetEntity(selectedEntityID);
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

			if (visibleEntityIDs.size() > 0)
			{
				ShowComponents(selectedEntity, ecs);

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

	void SceneInspectorPopUp::ShowInspector()
	{
	}

	void SceneInspectorPopUp::ShowComponents(ECS::Entity& aSelectedEntity, ECS::EntityComponentSystem& aActiveECS)
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
			SetComponentValueCommand setComponentValueCommand;
			setComponentValueCommand.myMemoryArena = &componentBufferArena;
			setComponentValueCommand.myMemoryHandle = copiedComponentMemoryHandle;
			setComponentValueCommand.myComponentHashCode = editedComponentHashCode;
			setComponentValueCommand.myComponentID = editedComponentID;

			CommandTracker& commandTracker = Editor::EditorProxy::GetCommandTracker();
			commandTracker.RegisterCommand(Command(setComponentValueCommand, "Set Component Value"));
		}

		previousFrameEditingAnyActive = anyComponentsActiveCurrentFrame;
	}
}
