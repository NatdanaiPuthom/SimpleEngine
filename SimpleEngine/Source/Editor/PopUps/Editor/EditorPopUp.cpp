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
	EditorPopUp::EditorPopUp(const std::string& aName)
		: PopUp(aName)
	{
	}

	void EditorPopUp::Init()
	{
	}

	void EditorPopUp::Render()
	{

	}
}
