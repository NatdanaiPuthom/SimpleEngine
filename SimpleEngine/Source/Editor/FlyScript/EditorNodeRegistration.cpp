#include "Editor/Precomplied/EditorPch.hpp"
#include "EditorNodeRegistration.hpp"

namespace Editor
{

	Fly::Flow EditorUpdate()
	{
		return Fly::Flow(true);
	}

	FLY_FUNCTION(EditorUpdate, "Events", Fly::Event{});
}