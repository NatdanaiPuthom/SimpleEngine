#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include "CustomEventWindow.hpp"
#include "FunctionWindow.hpp"
#include "FunctionSettingsWindow.hpp"
#include "FlyClassWindow.hpp"
#include "StructCreatorWindow.hpp"
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
	struct NodeTypeCategory
	{
		const std::string name;
		std::vector<NodeTypeCategory> childCategories;
		std::vector<Fly::NodeTypeFacade> nodeTypes;
	};

	inline constexpr unsigned int ToImGuiColor(const Fly::Color& aColor)
	{
		return IM_COL32(aColor.r * 255, aColor.g * 255, aColor.b * 255, 255);
	}

	struct NodeContext final
	{
		Fly::ClassFacade myClassFacade;
		Fly::NodeGraphFacade myNodeGraphFacade;
		Fly::ClassInstanceFacade myClassInstanceFacade;

		std::vector<Fly::PinFacade> myPinFacadesToHighlight;
		std::unordered_map<Fly::NodeID, Fly::NodeDragData> myNodeDragData;
		Fly::PinID myLinkCreationPinID;
		Fly::PinID myStartedLinkPinID;
		std::vector<Fly::LinkFacade> myTraversedLinks;
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
		void SetNodeContext(Fly::NodeGraphFacade aNodeGraphFacade, Fly::ClassFacade aClassFacade);
		eGraphMode GetCurrentMode() const;
		void SetSelectedFunctionFacade(Fly::FunctionFacade aFunctionFacade);

		bool OpenClassByName(std::string_view aName);

		void UpdateContext();

		void Draw() override;

		void ShowSelectionMenu();
		void ShowLoadingMenu();
		void VisualizeNodes();
		void UpdateNodes();
		void NodeCreation();

		ImVec2 GetMiddlePos() const;

		Fly::FunctionFacade GetCurrentFunctionFacade();

		Fly::CommandTracker& GetCommandTracker()
		{
			return *myCommandTracker;
		}

	private:
		bool ShowNodeSearchMenu(const std::vector<Fly::NodeTypeFacade>& aNodeTypeIDs);
		bool ShowNodeSearchMenuByCategory(const NodeTypeCategory& aCategory);
		void ShowNodeSearchMenu();
		void PopulateCategories(const std::string& aName, const Fly::NodeTypeFacade& aNodeType, NodeTypeCategory& aCategory);

		void UpdateClickPos();
		ImVec2 GetMousePos() const;

	private:
		char myNewClassNameText[TEXT_MAX_LENGTH] = "";
		Fly::DataTypeFacade mySelectedTargetDataType;
		char myCreateCopyNameText[TEXT_MAX_LENGTH]{};
		char myNodeTypeSearch[TEXT_MAX_LENGTH] = "";

		std::unique_ptr<Fly::CommandTracker> myCommandTracker;

		std::unordered_map<Fly::NodeGraphFacade, ImNodesContext*> myImNodesContexts;

		
		FlyClassWindow myClassWindow;
		CustomEventWindow myCustomEventWindow;
		FunctionSettingsWindow myFunctionSettingsWindow;
		StructCreatorWindow myStructCreatorWindow;

		NodeContextHistory myNodeContextHistory;

		ImVec2 myNodeCreationClickPos;

		bool myIsDraggingNode = false;
		ImVec2 myDragStartPos;
		ImVec2 myDragEndPos;

		Fly::NodeFacade myClickedNodeFacade;
		Fly::LinkFacade myHoveredLinkFacade;
		Fly::PinFacade myHoveredPinFacade;
		Fly::PinFacade myClickedPinFacade;

		struct SearchNodeData
		{
			std::function<void(NodeTypeCategory&)> myCategoryFunction;
			std::function<void(const Fly::NodeTypeFacade&)> myOnClickFunction;

			int myCurrentIndex = 0;
		};

		bool myIsContextSensitive = false;

		SearchNodeData mySearchNodeData;

		Fly::FunctionFacade mySelectedFunctionFacade;

		static constexpr Fly::Color myTraversedLinkColor{ 0.2f, 0.9f, 0.7f, 1.f };

		static constexpr Fly::Color mySelectionTint{ 0.2f, 0.2f, 0.2f, 0.f };// = Color(0.2f, 0.2f, 0.2f, 0);
		static constexpr Fly::Color myHoverTint{ 0.1f, 0.1f, 0.1f, 0.f };

		static constexpr const char* ASSET_FILE_PATH = "Assets/FlyClasses";
		static constexpr const char* NODE_SEARCH_POPUP_NAME = "NodeSearchPopup";

	};
}