#pragma once
#include "SimpleScript/Core/ScriptDefines.h"
#include "SimpleScriptEditor/VariableWindow.h"
#include "SimpleScriptEditor/NodeCreatorWindow.h"
#include "SimpleScriptEditor/FunctionWindow.h"
#include "SimpleScript/Core/SystemTypes/ScriptVec2.h"
#include <External/imgui.h>
#include <vector>
#include <string>
#include <functional>

struct ImNodesContext;

namespace SCRIPT
{
	struct Color;
}

namespace SCRIPT
{
	class ScriptManager;
	class Script;
	class CommandTracker;
}

namespace Editor
{
	struct NodeTypeCategory
	{
		const std::string name;
		std::vector<NodeTypeCategory> childCategories;
		std::vector<SCRIPT::NodeTypeID> nodeTypesIDs;
	};

	inline static unsigned int ToImGuiColor(const SCRIPT::Color& aColor)
	{
		return IM_COL32(aColor.r * 255, aColor.g * 255, aColor.b * 255, 255);
	}

	struct NodeContext
	{
		SCRIPT::Script* script;
		SCRIPT::NodeGraph* nodeGraph;
		ImNodesContext* imNodesContext;
	};

	inline SCRIPT::ScriptVec2 ToScriptVec2(ImVec2 aVec)
	{
		return { aVec.x, aVec.y };
	}

	constexpr size_t TEXT_MAX_LENGTH = 40;

	class VisualScriptingWindow
	{
	public:

		VisualScriptingWindow();
		~VisualScriptingWindow();

		NodeContext GetCurrentContext() const;
		void UpdateContext();

		void Update(SCRIPT::ScriptManager& aScriptManager, const std::string& LevelName);

		void ScriptSelectionMenu();
		void ScriptLoadingMenu();
		void VisualizeNodes();
		void UpdateNodes();
		void NodeCreation();

		ImVec2 GetMiddlePos() const;

	private:
		void ShowNodeTypeCreationMenu(const std::vector<SCRIPT::NodeTypeID>& aNodeTypeIDs, const std::function<void(SCRIPT::NodeTypeID)>& aOnClickFunc);
		void ShowNodeCreationMenuByCategory(const NodeTypeCategory& aCategory, const std::function<void(SCRIPT::NodeTypeID)>& aOnClickFunc);
		void ShowNodeCreationMenu(const std::function<void(NodeTypeCategory&)>& aCategoryFunction, const std::function<void(SCRIPT::NodeTypeID)>& aOnClickFunction);
		void PopulateCategories(const std::string& aName, const size_t aNodeTypeID, NodeTypeCategory& aCategory);

		void UpdateClickPos();
		ImVec2 GetMousePos() const;

	private:
		char myScriptNameText[TEXT_MAX_LENGTH] = "";
		char myNodeTypeSearch[TEXT_MAX_LENGTH] = "";

		SCRIPT::ScriptManager* myCurrentScriptManager;
		std::unique_ptr<SCRIPT::CommandTracker> myCommandTracker;

		size_t myCurrentIndex;
		std::vector<ImNodesContext*> myContexts;

		SCRIPT::PinID myLinkCreationPinID;
		SCRIPT::PinID myStartedLinkPinID;
		std::vector<SCRIPT::PinID> myPinIDsToHighlight;

		VariableWindow myVariableWindow;
		NodeCreatorWindow myNodeCreatorWindow;
		FunctionWindow myFunctionWindow;

		ImVec2 myNodeCreationClickPos;

		bool myIsDraggingNode;
		ImVec2 myDragStartPos;
		ImVec2 myDragEndPos;

		int myHoveredLinkID = SCRIPT::InvalidID<SCRIPT::PinID>();
	};
}