#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/EditorProxy.hpp"
#include "Editor/Utility/EditorUtilities.hpp"


namespace Editor
{
	void EditorProxy::SetSelectedEntityID(size_t aEntityID)
	{
		mySelectedEntityID = aEntityID;
	}

	void EditorProxy::SetSelectedEntityIDToInvalid()
	{
		mySelectedEntityID = GetInvalidIndex<size_t>();
	}

	CommandTracker& EditorProxy::GetCommandTracker()
	{
		return myEditorEngine->GetCommandTracker();
	}

	size_t EditorProxy::GetSelectedEntityID()
	{
		return mySelectedEntityID;
	}
}
