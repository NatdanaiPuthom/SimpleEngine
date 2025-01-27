#pragma once
#include "Editor/Core/PopUp.hpp"
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

	class NodeScriptingWindow : public PopUp
	{
	public:

		NodeScriptingWindow(const std::string& aName);
		~NodeScriptingWindow();

		NodeGraphContext& GetNodeContext();
		const NodeGraphContext& GetNodeContext() const;
		void SetNodeContext(Fly::NodeGraphProxy aNodeGraphProxy, Fly::ClassProxy aClassProxy);
		eGraphMode GetCurrentMode() const;
		void SetSelectedFunctionProxy(Fly::FunctionProxy aFunctionProxy);

		bool OpenClassByName(std::string_view aName);

		void UpdateContext();

		void Render() override;

		void ShowSelectionMenu();
		void ShowLoadingMenu();

		ImVec2 GetMiddlePos() const;

		Fly::FunctionProxy GetCurrentFunctionProxy();

		Fly::CommandTracker& GetCommandTracker()
		{
			return *GetNodeContext().myCommandTracker;
		}

	private:

		void UpdateClickPos();
		ImVec2 GetMousePos() const;

	private:
		char myNewClassNameText[NodeGraphContext::TEXT_MAX_LENGTH] = "";
		Fly::GenericDataTypeProxy mySelectedTargetDataType;
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
		
		Fly::FunctionProxy mySelectedFunctionProxy;

		static constexpr const char* ASSET_FILE_PATH = "Assets/FlyClasses";

	};
}