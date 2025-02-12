#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/EditorProxy.hpp"
#include "Editor/EditorEngine.hpp"
#include "Editor/Utility/EditorUtilities.hpp"
#include "MainSingleton/MainSingleton.hpp"

#ifndef _SIMPLE
namespace Editor
{
	void EditorProxy::Init(EditorEngine* aEditorEngine)
	{
		myEditorEngine = aEditorEngine;
		SetSelectedEntityIDToInvalid();
		SetSelectedEntityIndexToInvalid();

		{
			ECS::EntityComponentSystem& activeECS = MainSingleton::GetSceneManager().GetCurrentECS();
			std::vector<ECS::Entity>& entities = activeECS.GetAllEntities();

			myVisibleEntityIDs.resize(entities.size());

			for (size_t i = 0; i < myVisibleEntityIDs.size(); i++)
			{
				myVisibleEntityIDs[i] = entities[i].GetID();
			}
		}
	}

	void EditorProxy::SetSelectedEntityID(size_t aEntityID)
	{
		mySelectedEntityID = aEntityID;
	}

	void EditorProxy::SetSelectedEntityIndex(size_t aEntityIndex)
	{
		mySelectedEntityIndex = aEntityIndex;
	}

	void EditorProxy::SetSelectedEntityIDToInvalid()
	{
		mySelectedEntityID = GetInvalidIndex<size_t>();
	}

	void EditorProxy::SetSelectedEntityIndexToInvalid()
	{
		mySelectedEntityIndex = GetInvalidIndex<size_t>();
	}

	ECS::EntityComponentSystem& EditorProxy::GetEditorECS()
	{
		return myEditorEngine->GetEditorECS();
	}

	std::vector<size_t>& EditorProxy::GetVisibleEntityIDs()
	{
		return myVisibleEntityIDs;
	}

	CommandTracker& EditorProxy::GetCommandTracker()
	{
		return myEditorEngine->GetCommandTracker();
	}

	size_t EditorProxy::GetSelectedEntityID()
	{
		return mySelectedEntityID;
	}

	size_t EditorProxy::GetSelectedEntityIndex()
	{
		return mySelectedEntityIndex;
	}

	size_t& EditorProxy::GetSelectedEntityIndexRef()
	{
		return mySelectedEntityIndex;
	}
}
#endif