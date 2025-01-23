#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include "CustomEventWindow.hpp"
#include "FunctionWindow.hpp"
#include "FunctionSettingsWindow.hpp"
#include "FlyClassWindow.hpp"
#include "StructCreatorWindow.hpp"
#include "FlyReflectionMemoryWindow.hpp"
#include "FlyTraitWindow.hpp"
#include "FlyScriptEditorUtilities.hpp"
#include "NodeScript/Fly.hpp"
#include <External/imgui.h>
#include <vector>
#include <string>
#include <functional>

struct ImNodesContext;

namespace Fly
{
	class CommandTracker;
}

namespace Editor
{

	struct NodeContextHistory
	{
		int currentIndex = -1;
		std::vector<std::unique_ptr<NodeGraphContext>> history;
	};


	enum class eGraphMode
	{
		Class,
		Global
	};

	class NodeScriptingWindow : public ToolInterface
	{
	public:

		NodeScriptingWindow();
		~NodeScriptingWindow();

		NodeGraphContext& GetNodeContext();
		const NodeGraphContext& GetNodeContext() const;
		void SetNodeContext(Fly::NodeGraphFacade aNodeGraphFacade, Fly::ClassFacade aClassFacade);
		eGraphMode GetCurrentMode() const;
		void SetSelectedFunctionFacade(Fly::FunctionFacade aFunctionFacade);

		bool OpenClassByName(std::string_view aName);

		void UpdateContext();

		void Draw() override;

		void ShowSelectionMenu();
		void ShowLoadingMenu();

		ImVec2 GetMiddlePos() const;

		Fly::FunctionFacade GetCurrentFunctionFacade();

		Fly::CommandTracker& GetCommandTracker()
		{
			return *GetNodeContext().myCommandTracker;
		}

	private:

		void UpdateClickPos();
		ImVec2 GetMousePos() const;

	private:
		char myNewClassNameText[NodeGraphContext::TEXT_MAX_LENGTH] = "";
		Fly::GenericDataTypeFacade mySelectedTargetDataType;
		char myCreateCopyNameText[NodeGraphContext::TEXT_MAX_LENGTH]{};

		FlyClassWindow myClassWindow;
		CustomEventWindow myCustomEventWindow;
		FunctionSettingsWindow myFunctionSettingsWindow;
		StructCreatorWindow myStructCreatorWindow;
		FlyReflectionMemoryWindow myReflectionMemoryWindow;
		FlyTraitWindow myTraitWindow;

		NodeContextHistory myNodeContextHistory;


		bool myIsContextSensitive = false;
		bool myIsNodeEditorHovered = false;
		
		Fly::FunctionFacade mySelectedFunctionFacade;

		static constexpr const char* ASSET_FILE_PATH = "Assets/FlyClasses";

	};
}