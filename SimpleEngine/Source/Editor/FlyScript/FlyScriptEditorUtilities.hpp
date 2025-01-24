#pragma once
#include "NodeScript/Fly.hpp"
#include <functional>
#include <memory>
#include "ImGui/imgui.h"

namespace Fly
{
	class DataTypeProxy;
	class GenericDataTypeProxy;
	class NodeGraphProxy;
	class CommandTracker;
}

namespace ImGui
{

	template<size_t MaxSize>
	bool InputString(const char* aText, std::string& aString)
	{
		char buffer[MaxSize]{};
		strcpy_s(buffer, aString.c_str());
		if (ImGui::InputText(aText, buffer, MaxSize))
		{
			aString = buffer;
			return true;
		}

		return false;
	}
}

struct ImNodesContext;


namespace Editor
{
	struct NodeTypeCategory final
	{
		std::string myName;
		std::vector<NodeTypeCategory> myChildCategories;
		std::vector<Fly::NodeTypeProxy> myNodeTypes;
	};

	struct NodeGraphContext final
	{
		static constexpr size_t TEXT_MAX_LENGTH = 40;
		static constexpr Fly::Color TRAVERSED_LINK_COLOR{ 0.2f, 0.9f, 0.7f, 1.f };
		static constexpr Fly::Color SELECTION_TINT{ 0.2f, 0.2f, 0.2f, 0.f };
		static constexpr Fly::Color HOVER_TINT{ 0.1f, 0.1f, 0.1f, 0.f };
		static constexpr const char* NODE_SEARCH_POPUP_NAME = "NodeSearchPopup";

		NodeGraphContext();
		~NodeGraphContext();

		ImNodesContext* myImNodesContext = nullptr;
		Fly::ClassProxy myClassProxy;
		Fly::NodeGraphProxy myNodeGraphProxy;
		Fly::ClassInstanceProxy myClassInstanceProxy;

		std::vector<Fly::PinProxy> myPinProxysToHighlight;
		std::unordered_map<Fly::NodeID, Fly::NodeDragData> myNodeDragData;
		Fly::PinID myLinkCreationPinID;
		Fly::PinID myStartedLinkPinID;
		Fly::NodeProxy myClickedNodeProxy;
		Fly::LinkProxy myHoveredLinkProxy;
		Fly::PinProxy myHoveredPinProxy;
		Fly::PinProxy myClickedPinProxy;
		std::vector<Fly::LinkProxy> myTraversedLinks;
		std::unique_ptr<Fly::CommandTracker> myCommandTracker;

		bool myIsDraggingNode = false;
		ImVec2 myDragStartPos;
		ImVec2 myDragEndPos;
		bool myIsNodeEditorHovered = false;

		struct SearchNodeData
		{
			std::function<void(NodeTypeCategory&)> myCategoryFunction;
			std::function<void(const Fly::NodeTypeProxy&)> myOnClickFunction;

			int myCurrentIndex = 0;
			char myNodeTypeSearch[TEXT_MAX_LENGTH] = "";
		};

		SearchNodeData mySearchNodeData;
		ImVec2 myNodeCreationClickPos;
	};

	template<typename T>
	constexpr std::string Combine(const char* aName, const T& aValue)
	{
		return std::string(aName) + std::to_string(aValue);
	}

	bool DataTypeComboEditableFilter(const char* aComboLabel, Fly::GenericDataTypeProxy& aDataTypeProxy);
	bool DataTypeComboTargetableFilter(const char* aComboLabel, Fly::GenericDataTypeProxy& aDataTypeProxy);
	bool DataTypeComboNoFilter(const char* aComboLabel, Fly::GenericDataTypeProxy& aDataTypeProxy);

	static bool StringCompare(std::string_view aStr1, std::string_view aStr2)
	{
		auto it = std::search(
			aStr1.begin(), aStr1.end(),
			aStr2.begin(), aStr2.end(),
			[](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
		);
		return it != aStr1.end();
	}

	constexpr unsigned int ToImGuiColor(const Fly::Color& aColor)
	{
		return IM_COL32(aColor.r * 255, aColor.g * 255, aColor.b * 255, 255);
	}

	constexpr Fly::Vec2 ToFlyVec2(ImVec2 aVec)
	{
		return { aVec.x, aVec.y };
	}

	template<typename T>
	concept Iteratable = requires(T && a)
	{
		{ begin(a) };
		{ end(a) };
	};

	template<Iteratable T, Iteratable U>
	inline bool SearchString(T&& aSearchIn, U&& aSearchFor)
	{
		auto it = std::search(begin(aSearchIn), end(aSearchIn), std::boyer_moore_horspool_searcher(begin(aSearchFor), end(aSearchFor)));

		return it != end(aSearchIn);
	}

	void UpdateClickPos(NodeGraphContext& aNodeGraphContext);
	ImVec2 GetMousePos(const NodeGraphContext& aNodeGraphContext);
	
	void ShowNodeGraph(NodeGraphContext& aNodeGraphContext);
	void VisualizeNodeGraph(NodeGraphContext& aNodeGraphContext);
	void UpdateNodeGraph(NodeGraphContext& aNodeGraphProxy);

	void NodeCreation(NodeGraphContext& aNodeGraphContext);
	bool ShowNodeSearchMenu(const std::vector<Fly::NodeTypeProxy>& aNodeTypes, const NodeGraphContext& aNodeGraphContext);
	bool ShowNodeSearchMenuByCategory(const NodeTypeCategory& aCategory, const NodeGraphContext& aNodeGraphContext);
	void ShowNodeSearchMenu(NodeGraphContext& aNodeGraphContext);
	void PopulateNodeCategories(const std::string& aName, const Fly::NodeTypeProxy& aNodeType, NodeTypeCategory& aCategory);


}
