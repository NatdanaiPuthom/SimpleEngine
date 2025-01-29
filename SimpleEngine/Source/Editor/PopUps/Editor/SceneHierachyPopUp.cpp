#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/SceneHierachyPopUp.hpp"
#include "Editor/Utility/EditorUtilities.hpp"
#include "Editor/EditorEngine.hpp" //TO-DO(v12.0.0) Temp
#include "Editor/Command/Commands/SelectEntityCommand.hpp"
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
			myEntityIDs->erase(begin(*myEntityIDs) + myHierarchyIndex);
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
		mySelectedEntityIndex = GetInvalidIndex<size_t>();
	}

	void SceneHierachyPopUp::Init()
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

	void SceneHierachyPopUp::Render()
	{
		static ECS::EntityID tempEntityID = GetInvalidIndex<ECS::EntityID>();

		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();

		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			ShowAddButtons();

			ImGui::Separator();

			ShowActiveSceneName();

			ImGui::Separator();

			ShowSceneEntities();
		}

		ImGui::End();

		if (mySelectedEntityIndex < 0)
		{
			mySelectedEntityIndex = 0;
		}
		else if (mySelectedEntityIndex >= myVisibleEntityIDs.size())
		{
			mySelectedEntityIndex = myVisibleEntityIDs.size() - 1;
		}

		if (myVisibleEntityIDs.empty() == false) //TO-DO(v12.0.0): Refactor, shouldn't be using from EditorEngine but EditorProxy if necessary
		{
			EditorEngine::mySelectedEntityID = myVisibleEntityIDs[mySelectedEntityIndex];
		}
		else
		{
			EditorEngine::mySelectedEntityID = GetInvalidIndex<ECS::EntityID>();
		}

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

		if (MainSingleton::GetInputManager().IsKeyPressed('K')) //NOTE(v12.0.0): Was this test function?
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
						ECS::EntityID createdEntityID = tempEntity.DuplicateThisToAnotherECS(ecs.GetEntityManager());

						myVisibleEntityIDs.push_back(createdEntityID);

						SelectEntity(myVisibleEntityIDs.size());
					}
				}
			}
		}
	}

	void SceneHierachyPopUp::ShowAddButtons()
	{
		static const std::string addButton = "Add" + myImGuiTag;
		static const std::string addSceneObjectButton = "Add Scene Object" + myImGuiTag;

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

				SelectEntity(static_cast<int>(myVisibleEntityIDs.size()));

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

				SelectEntity(static_cast<int>(myVisibleEntityIDs.size()));

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

		const ImVec2 parentSize = ImGui::GetContentRegionAvail();

		if (ImGui::BeginListBox(listBoxName.c_str(), parentSize))
		{
			ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();

			for (int i = 0; i < myVisibleEntityIDs.size(); ++i)
			{
				ECS::Entity& entity = ecs.GetEntity(myVisibleEntityIDs[i]);

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
					ImGui::OpenPopup(entityPropertyPopUp.c_str());
				}

				if (ImGui::BeginPopup(entityPropertyPopUp.c_str()))
				{
					if (ImGui::MenuItem(removeItem.c_str()))
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

	ECS::EntityID SceneHierachyPopUp::CreateEntity()
	{
		ECS::EntityID entityID = MainSingleton::GetSceneManager().GetCurrentECS().CreateEntity();

		CreateEntityCommand createEntityCommand;
		createEntityCommand.myHierarchyIndex = myVisibleEntityIDs.size();
		createEntityCommand.myEntityIDs = &myVisibleEntityIDs;
		createEntityCommand.myEntityID = entityID;

		myCommandTracker->ExecuteCommand(Command(createEntityCommand, "Create Entity"));

		return entityID;
	}

	void SceneHierachyPopUp::SelectEntity(size_t aNewIndex)
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

	void SceneHierachyPopUp::RemoveEntity(size_t aEntityIndex)
	{
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

	void SceneHierachyPopUp::RemoveSelectedEntity()
	{
		RemoveEntity(mySelectedEntityIndex);
	}

	ECS::Entity* SceneHierachyPopUp::GetSelectedEntity() const
	{
		if (mySelectedEntityIndex == GetInvalidIndex<size_t>())
		{
			return nullptr;
		}

		return &MainSingleton::GetSceneManager().GetCurrentECS().GetEntity(myVisibleEntityIDs[mySelectedEntityIndex]);
	}
}
