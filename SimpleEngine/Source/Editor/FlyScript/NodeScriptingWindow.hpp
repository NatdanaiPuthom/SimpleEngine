#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include "NodeScript/SimpleScript/Core/ScriptDefines.h"
#include "VariableWindow.hpp"
#include "NodeCreatorWindow.hpp"
#include "FunctionWindow.hpp"
#include "FunctionSettingsWindow.hpp"
#include "NodeScript/SimpleScript/Core/SystemTypes/ScriptVec2.h"
#include "NodeScript/SimpleScript/Core/ScriptModifier.h"
#include <External/imgui.h>
#include <vector>
#include <string>
#include <functional>

struct ImNodesContext;

namespace SCRIPT
{
	struct Color;
	class ScriptManager;
	class Script;
	class CommandTracker;
	class NodeGraph;
}

namespace Editor
{
	struct NodeTypeCategory
	{
		const std::string name;
		std::vector<NodeTypeCategory> childCategories;
		std::vector<SCRIPT::NodeTypeView> nodeTypes;
	};

	inline static unsigned int ToImGuiColor(const SCRIPT::Color& aColor)
	{
		return IM_COL32(aColor.r * 255, aColor.g * 255, aColor.b * 255, 255);
	}

	struct NodeContext
	{
		SCRIPT::Script* script;
		SCRIPT::NodeGraph* nodeGraph;

		std::vector<SCRIPT::PinID> myPinIDsToHighlight;
		std::unordered_map<SCRIPT::NodeID, SCRIPT::NodeDragData> myNodeDragData;
		SCRIPT::PinID myLinkCreationPinID;
		SCRIPT::PinID myStartedLinkPinID;
	};

	struct NodeContextHistory
	{
		int currentIndex = -1;
		std::vector<NodeContext> history;
	};

	inline SCRIPT::Vec2 ToScriptVec2(ImVec2 aVec)
	{
		return { aVec.x, aVec.y };
	}

	constexpr size_t TEXT_MAX_LENGTH = 40;

	enum class eScriptMode
	{
		Class,
		Global
	};

	class NodeScriptingWindow : public ToolInterface
	{
	public:

		NodeScriptingWindow();
		~NodeScriptingWindow();

		NodeContext& GetNodeContext();
		const NodeContext& GetNodeContext() const;
		void SetNodeContext(SCRIPT::NodeGraph& aNodeGraph, SCRIPT::Script* aScript);
		eScriptMode GetCurrentMode() const;

		void UpdateContext();

		void Draw() override;

		void ScriptSelectionMenu();
		void ScriptLoadingMenu();
		void VisualizeNodes();
		void UpdateNodes();
		void NodeCreation();

		ImVec2 GetMiddlePos() const;

		SCRIPT::FunctionID GetCurrentFunctionID() const;

	private:
		void ShowNodeTypeCreationMenu(const std::vector<SCRIPT::NodeTypeView>& aNodeTypeIDs, const std::function<void(const SCRIPT::NodeTypeView&)>& aOnClickFunc);
		void ShowNodeCreationMenuByCategory(const NodeTypeCategory& aCategory, const std::function<void(const SCRIPT::NodeTypeView&)>& aOnClickFunc);
		void ShowNodeCreationMenu(const std::function<void(NodeTypeCategory&)>& aCategoryFunction, const std::function<void(const SCRIPT::NodeTypeView&)>& aOnClickFunction);
		void PopulateCategories(const std::string& aName, const SCRIPT::NodeTypeView& aNodeType, NodeTypeCategory& aCategory);

		void UpdateClickPos();
		ImVec2 GetMousePos() const;

	private:
		char myScriptNameText[TEXT_MAX_LENGTH] = "";
		char myNodeTypeSearch[TEXT_MAX_LENGTH] = "";

		std::unique_ptr<SCRIPT::CommandTracker> myCommandTracker;

		std::unordered_map<const SCRIPT::NodeGraph*, ImNodesContext*> myImNodesContexts;

		

		VariableWindow myVariableWindow;
		NodeCreatorWindow myNodeCreatorWindow;
		FunctionWindow myFunctionWindow;
		FunctionSettingsWindow myFunctionSettingsWindow;

		NodeContextHistory myNodeContextHistory;

		ImVec2 myNodeCreationClickPos;

		bool myIsDraggingNode;
		ImVec2 myDragStartPos;
		ImVec2 myDragEndPos;

		SCRIPT::PinID myHoveredLinkID = SCRIPT::InvalidID<SCRIPT::PinID>();
		SCRIPT::PinID myHoveredPinID = SCRIPT::InvalidID<SCRIPT::PinID>();

		bool myIsContextSensitive = false;

		SCRIPT::FunctionID mySelectedFunctionID = SCRIPT::InvalidID<SCRIPT::FunctionID>();

		static constexpr const char* SCRIPT_FILE_PATH = "Assets/Scripts/";

	};
}