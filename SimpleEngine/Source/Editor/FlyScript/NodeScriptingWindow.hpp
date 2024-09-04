#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include "NodeScript/SimpleScript/Core/FlyDefines.hpp"
#include "VariableWindow.hpp"
#include "CustomEventWindow.hpp"
#include "FunctionWindow.hpp"
#include "FunctionSettingsWindow.hpp"
#include "NodeScript/SimpleScript/Core/Fly.hpp"
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
	struct NodeTypeCategory
	{
		const std::string name;
		std::vector<NodeTypeCategory> childCategories;
		std::vector<Fly::NodeTypeView> nodeTypes;
	};

	inline constexpr unsigned int ToImGuiColor(const Fly::Color& aColor)
	{
		return IM_COL32(aColor.r * 255, aColor.g * 255, aColor.b * 255, 255);
	}

	struct NodeContext final
	{
		Fly::ClassView myClassView;
		Fly::NodeGraphView myNodeGraphView;
		Fly::ClassInstanceView myClassInstanceView;

		std::vector<Fly::PinView> myPinViewsToHighlight;
		std::unordered_map<Fly::NodeID, Fly::NodeDragData> myNodeDragData;
		Fly::PinID myLinkCreationPinID;
		Fly::PinID myStartedLinkPinID;
	};

	struct NodeContextHistory
	{
		int currentIndex = -1;
		std::vector<NodeContext> history;
	};

	inline Fly::Vec2 ToFlyVec2(ImVec2 aVec)
	{
		return { aVec.x, aVec.y };
	}

	constexpr size_t TEXT_MAX_LENGTH = 40;

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

		NodeContext& GetNodeContext();
		const NodeContext& GetNodeContext() const;
		void SetNodeContext(Fly::NodeGraphView aNodeGraphView, Fly::ClassView aClassView);
		eGraphMode GetCurrentMode() const;

		void UpdateContext();

		void Draw() override;

		void ShowSelectionMenu();
		void ShowLoadingMenu();
		void VisualizeNodes();
		void UpdateNodes();
		void NodeCreation();

		ImVec2 GetMiddlePos() const;

		Fly::FunctionView GetCurrentFunction() const;

		Fly::CommandTracker& GetCommandTracker()
		{
			return *myCommandTracker;
		}

	private:
		void ShowNodeTypeCreationMenu(const std::vector<Fly::NodeTypeView>& aNodeTypeIDs, const std::function<void(const Fly::NodeTypeView&)>& aOnClickFunc);
		void ShowNodeCreationMenuByCategory(const NodeTypeCategory& aCategory, const std::function<void(const Fly::NodeTypeView&)>& aOnClickFunc);
		void ShowNodeCreationMenu(const std::function<void(NodeTypeCategory&)>& aCategoryFunction, const std::function<void(const Fly::NodeTypeView&)>& aOnClickFunction);
		void PopulateCategories(const std::string& aName, const Fly::NodeTypeView& aNodeType, NodeTypeCategory& aCategory);

		void UpdateClickPos();
		ImVec2 GetMousePos() const;

	private:
		char myNewClassNameText[TEXT_MAX_LENGTH] = "";
		Fly::DataTypeView mySelectedTargetDataType;
		char myCreateCopyNameText[TEXT_MAX_LENGTH]{};
		char myNodeTypeSearch[TEXT_MAX_LENGTH] = "";

		std::unique_ptr<Fly::CommandTracker> myCommandTracker;

		std::unordered_map<Fly::NodeGraphView, ImNodesContext*> myImNodesContexts;

		

		VariableWindow myVariableWindow;
		CustomEventWindow myNodeCreatorWindow;
		FunctionWindow myFunctionWindow;
		FunctionSettingsWindow myFunctionSettingsWindow;

		NodeContextHistory myNodeContextHistory;

		ImVec2 myNodeCreationClickPos;

		bool myIsDraggingNode;
		ImVec2 myDragStartPos;
		ImVec2 myDragEndPos;

		Fly::NodeView myClickedNodeView;
		Fly::LinkView myHoveredLinkView;
		Fly::PinView myHoveredPinView;
		Fly::PinView myClickedPinView;

		bool myIsContextSensitive = false;

		Fly::FunctionView mySelectedFunction;

		static constexpr Fly::Color mySelectionTint{ 0.2f, 0.2f, 0.2f, 0.f };// = Color(0.2f, 0.2f, 0.2f, 0);
		static constexpr Fly::Color myHoverTint{ 0.1f, 0.1f, 0.1f, 0.f };

		static constexpr const char* ASSET_FILE_PATH = "Assets/Classes";

	};
}