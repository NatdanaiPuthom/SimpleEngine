#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/SceneHierachyPopUp.hpp"
#include "Editor/EditorProxy.hpp"
#include "Editor/Utility/EditorUtilities.hpp"
#include "Editor/Command/Commands/SelectEntityCommand.hpp"
#include "Editor/Command/Core/EditorCommandTracker.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	class RemoveEntityCommand final //TO-Do(v12.0.0): Move to own Command file
	{
	public:
		size_t myHierarchyIndex = GetInvalidIndex<size_t>();
		ECS::EntityID myEntityID = GetInvalidIndex<ECS::EntityID>();
		std::vector<ECS::EntityID>* myEntityIDs = nullptr;

		void Execute() const
		{
			const size_t entityIDToRemove = (*myEntityIDs)[myHierarchyIndex];
			myEntityIDs->erase(begin(*myEntityIDs) + myHierarchyIndex);

			ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
			ecs.RemoveEntity(entityIDToRemove);
		}

		void Undo() const
		{
			myEntityIDs->insert(begin(*myEntityIDs) + myHierarchyIndex, myEntityID);
		}
	};

	class CreateEntityCommand final //TO-Do(v12.0.0): Move to own Command file
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
}

namespace Editor
{
	SceneHierachyPopUp::SceneHierachyPopUp(const std::string& aName, CommandTracker* aCommandTracker)
		: PopUp(aName)
		, myCommandTracker(aCommandTracker)
	{
	}

	void SceneHierachyPopUp::Render()
	{
		static ECS::EntityID tempEntityID = GetInvalidIndex<ECS::EntityID>();

		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::EntityComponentSystem& editorECS = EditorProxy::GetEditorECS();

		std::vector<size_t>& visibleEntityIDs = EditorProxy::GetVisibleEntityIDsRef();

		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			ShowAddButtons();

			ImGui::Separator();

			ShowActiveSceneName();

			ImGui::Separator();

			ShowSceneEntities();
		}

		ImGui::End();

		if (EditorProxy::GetSelectedEntityIndex() < 0)
		{
			EditorProxy::SetSelectedEntityIndex(0);
		}
		else if (EditorProxy::GetSelectedEntityIndex() >= visibleEntityIDs.size())
		{
			EditorProxy::SetSelectedEntityIndex(visibleEntityIDs.size() - 1);
		}

		if (visibleEntityIDs.empty() == false)
		{
			EditorProxy::SetSelectedEntityID(visibleEntityIDs[EditorProxy::GetSelectedEntityIndex()]);
		}
		else
		{
			EditorProxy::SetSelectedEntityIDToInvalid();
		}

		if (MainSingleton::GetInputManager().IsKeyPressed(VK_DELETE))
		{
			if (!visibleEntityIDs.empty())
			{
				if (EditorProxy::GetSelectedEntityIndex() >= 0)
				{
					RemoveSelectedEntity();
					EditorProxy::SetSelectedEntityIDToInvalid();
					return;
				}
			}
		}

		if (MainSingleton::GetInputManager().IsKeyPressed('K')) //NOTE(v12.0.0): Was this test function?
		{
			if (!visibleEntityIDs.empty())
			{
				if (EditorProxy::GetSelectedEntityIndex() >= 0)
				{
					ECS::EntityID createdEntityID = GetSelectedEntity()->DuplicateThis();
					visibleEntityIDs.insert(begin(visibleEntityIDs) + EditorProxy::GetSelectedEntityIndex() + 1, createdEntityID);
				}
			}
		}

		if (MainSingleton::GetInputManager().IsKeyHeld(VK_CONTROL) && MainSingleton::GetInputManager().IsKeyPressed('C'))
		{
			if (!visibleEntityIDs.empty())
			{
				if (EditorProxy::GetSelectedEntityIndex() >= 0)
				{
					tempEntityID = GetSelectedEntity()->DuplicateThisToAnotherECS(editorECS.GetEntityManager());
				}
			}
		}
		else if (MainSingleton::GetInputManager().IsKeyHeld(VK_CONTROL) && MainSingleton::GetInputManager().IsKeyPressed('V'))
		{
			if (!visibleEntityIDs.empty())
			{
				if (EditorProxy::GetSelectedEntityIndex() >= 0)
				{
					if (tempEntityID != GetInvalidIndex<ECS::EntityID>())
					{
						ECS::Entity& tempEntity = editorECS.GetEntity(tempEntityID);
						ECS::EntityID createdEntityID = tempEntity.DuplicateThisToAnotherECS(ecs.GetEntityManager());

						visibleEntityIDs.push_back(createdEntityID);

						SelectEntity(visibleEntityIDs.size());
					}
				}
			}
		}
	}

	void SceneHierachyPopUp::ShowAddButtons()
	{
		static const std::string addButton = "Add" + myImGuiTag;
		static const std::string addSceneObjectButton = "Add Scene Object" + myImGuiTag;

		std::vector<size_t>& visibleEntityIDs = EditorProxy::GetVisibleEntityIDsRef();

		if (ImGui::Button(addButton.c_str()))
		{
			ImGui::OpenPopup(addSceneObjectButton.c_str());
		}

		if (ImGui::BeginPopup(addSceneObjectButton.c_str()))
		{
			static const std::string addEntityItem = "Add Entity" + myImGuiTag;
			static const std::string addCubeItem = "Add Cube" + myImGuiTag;

			if (ImGui::MenuItem(addEntityItem.c_str()))
			{
				myCommandTracker->BeginComposite("Create Entity Composite");

				ECS::EntityComponentSystem& activeECS = MainSingleton::GetSceneManager().GetCurrentECS();
				ECS::EntityID createdEntityID = CreateEntity();
				ECS::Entity& entity = activeECS.GetEntity(createdEntityID);

				entity.AddComponent<ECS::TransformComponent>();

				SelectEntity(static_cast<int>(visibleEntityIDs.size()));

				myCommandTracker->EndComposite();
			}

			if (ImGui::MenuItem(addCubeItem.c_str()))
			{
				myCommandTracker->BeginComposite("Create Entity Composite");

				ECS::EntityComponentSystem& activeECS = MainSingleton::GetSceneManager().GetCurrentECS();
				ECS::EntityID createdEntityID = CreateEntity();
				ECS::Entity& entity = activeECS.GetEntity(createdEntityID);

				entity.AddComponent<ECS::TransformComponent>();
				entity.AddComponent<ECS::MeshComponent>();

				SelectEntity(static_cast<int>(visibleEntityIDs.size()));

				myCommandTracker->EndComposite();
			}

			ImGui::EndPopup();
		}
	}

	void SceneHierachyPopUp::ShowActiveSceneName()
	{
		static const std::string sceneInputWithTextButton = "Scene" + myImGuiTag;

		const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();

		char sceneName[256]{};
		memset(sceneName, '\0', sizeof(sceneName));
		strncpy_s(sceneName, sceneInfo->name.c_str(), sizeof(sceneName));
		sceneName[sizeof(sceneName) - 1] = '\0';

		if (ImGui::InputTextWithHint(sceneInputWithTextButton.c_str(), "Name", sceneName, sizeof(sceneName)))
		{
			if (ImGui::IsItemFocused() && MainSingleton::GetInputManager().IsKeyPressed(VK_RETURN))
			{
				std::string newSceneName(sceneName);
				MainSingleton::GetSceneManager().ChangeSceneName(newSceneName);
			}
		}
	}

	void SceneHierachyPopUp::ShowSceneEntities()
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0.18f, 0.18f, 0.18f, 0.80f).Value);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0.12f, 0.12f, 0.12f, 0.0f).Value);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

		static const std::string listBoxName = "" + myImGuiTag;
		static const std::string entityPropertyPopUp = "Entity Property" + myImGuiTag;
		static const std::string removeItem = "Remove" + myImGuiTag;

		std::vector<size_t>& visibleEntityIDs = EditorProxy::GetVisibleEntityIDsRef();

		const ImVec2 parentSize = ImGui::GetContentRegionAvail();

		if (ImGui::BeginListBox(listBoxName.c_str(), parentSize))
		{
			ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();

			for (int i = 0; i < visibleEntityIDs.size(); ++i)
			{
				ECS::Entity& entity = ecs.GetEntity(visibleEntityIDs[i]);

				const bool isSelected = (EditorProxy::GetSelectedEntityIndex() == i);

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
					ImGui::OpenPopup(entityPropertyPopUp.c_str());
				}

				if (ImGui::BeginPopup(entityPropertyPopUp.c_str()))
				{
					if (ImGui::MenuItem(removeItem.c_str()))
					{
						RemoveEntity(EditorProxy::GetSelectedEntityIndex());
						EditorProxy::SetSelectedEntityIDToInvalid();
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

	ECS::EntityID SceneHierachyPopUp::CreateEntity()
	{
		ECS::EntityID entityID = MainSingleton::GetSceneManager().GetCurrentECS().CreateEntity();

		CreateEntityCommand createEntityCommand;
		createEntityCommand.myHierarchyIndex = EditorProxy::GetVisibleEntityIDsRef().size();
		createEntityCommand.myEntityIDs = &EditorProxy::GetVisibleEntityIDsRef();
		createEntityCommand.myEntityID = entityID;

		myCommandTracker->ExecuteCommand(Command(createEntityCommand, "Create Entity"));

		return entityID;
	}

	void SceneHierachyPopUp::SelectEntity(size_t aNewIndex)
	{
		if (aNewIndex == EditorProxy::GetSelectedEntityIndex())
		{
			return;
		}

		SelectEntityCommand selectEntityCommand;
		selectEntityCommand.myNewIndex = aNewIndex;
		selectEntityCommand.myOldIndex = EditorProxy::GetSelectedEntityIndex();
		selectEntityCommand.mySelectedEntityIndexPtr = &EditorProxy::GetSelectedEntityIndexRef();

		myCommandTracker->ExecuteCommand(Command(selectEntityCommand, "Select Entity"));
	}

	void SceneHierachyPopUp::RemoveEntity(size_t aEntityIndex)
	{
		std::vector<size_t>& visibleEntityIDs = EditorProxy::GetVisibleEntityIDsRef();

		RemoveEntityCommand removeEntityCommand;
		removeEntityCommand.myHierarchyIndex = aEntityIndex;
		removeEntityCommand.myEntityIDs = &visibleEntityIDs;
		removeEntityCommand.myEntityID = visibleEntityIDs[aEntityIndex];

		myCommandTracker->BeginComposite("Remove Entity Composite");

		myCommandTracker->ExecuteCommand(Command(removeEntityCommand, "Remove Entity"));

		size_t newSelectedIndex = EditorProxy::GetSelectedEntityIndex();

		if (EditorProxy::GetSelectedEntityIndex() >= visibleEntityIDs.size() - 1)
		{
			newSelectedIndex = EditorProxy::GetSelectedEntityIndex() - 1;
		}

		SelectEntity(newSelectedIndex);

		if (EditorProxy::GetSelectedEntityIndex() < 0 && visibleEntityIDs.size() > 0)
		{
			EditorProxy::SetSelectedEntityIndex(0);
		}

		myCommandTracker->EndComposite();
	}

	void SceneHierachyPopUp::RemoveSelectedEntity()
	{
		RemoveEntity(EditorProxy::GetSelectedEntityIndex());
	}

	ECS::Entity* SceneHierachyPopUp::GetSelectedEntity() const
	{
		if (EditorProxy::GetSelectedEntityIndex() == GetInvalidIndex<size_t>())
		{
			return nullptr;
		}

		const std::vector<size_t>& visibleEntityIDs = EditorProxy::GetVisibleEntityIDsRef();

		return &MainSingleton::GetSceneManager().GetCurrentECS().GetEntity(visibleEntityIDs[EditorProxy::GetSelectedEntityIndex()]);
	}
}
