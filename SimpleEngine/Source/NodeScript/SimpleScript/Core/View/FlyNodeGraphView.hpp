#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeView.hpp"
#include "FlyPinView.hpp"
#include "FlyLinkView.hpp"
#include "FlyDataTypeView.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"

namespace FLY_NAMESPACE
{

	struct NodeDragData final
	{
		Vec2 mStartPos;
		Vec2 mEndPos;
	};
	
	class NodeGraph;
	class EventGraph;
	class FunctionView;
	class ClassView;
	class NodeTypeView;
	class VariableView;


	enum class eNodeGraphType
	{
		EventGraph,
		Function
	};

	class NodeGraphView final
	{
	public:

		NodeGraphView() = default;
		explicit NodeGraphView(const FunctionView& aFunctionView);
		explicit NodeGraphView(EventGraph& aEventGraph);
		explicit NodeGraphView(NodeGraphVariant&& aNodeGraphVariant);
		explicit NodeGraphView(const NodeGraphVariant& aNodeGraphVariant);

		[[nodiscard]] std::vector<NodeView> GetNodeViews(bool aIncludeDestroyed = false) const;
		[[nodiscard]] std::vector<PinView> GetPinViews(bool aIncludeDestroyed = false) const;

		[[nodiscard]] NodeGraph& GetNodeGraph();
		[[nodiscard]] const NodeGraph& GetNodeGraph() const;

		[[nodiscard]] eNodeGraphType GetType() const;

		[[nodiscard]] std::vector<PinView> GetNonConnectedInputPinViews() const;
		[[nodiscard]] std::vector<PinView> GetNonConnectedOutputPinViews() const;
		[[nodiscard]] std::vector<PinView> GetNonConnectedPinViewsByFlowType(eFlowType aFlowType) const;
		[[nodiscard]] std::vector<PinView> GetNonConnectedPinViewsByFlowTypeAndDataType(eFlowType aFlowType, DataTypeView aDataTypeView) const;

		NodeView CreateNode(const NodeTypeView& aNodeTypeView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeView CreateNode(std::string_view aName, bool& aSuccess, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
		NodeView CreateNodeAutoLink(NodeTypeView aNodeTypeView, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeView CreateGetterNode(VariableView aVariableView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeView CreateSetterNode(VariableView aVariableView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, CommandTracker* aCommandTracker);
		LinkView TryCreateLink(PinView aPinView1, PinView aPinView2, CommandTracker* aCommandTracker);

		void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, CommandTracker* aCommandTracker);

		void ReplaceTemplateNode(NodeView aReplaceNodeView, DataTypeView aDataTypeView, CommandTracker* aCommandTracker);
		void ReplaceTemplateNode(PinView aReplacePinView, DataTypeView aDataTypeView, CommandTracker* aCommandTracker);

		[[nodiscard]] const NodeGraphVariant& GetVariant() const;

		friend bool operator==(const NodeGraphView& a, const NodeGraphView& b);

		explicit operator bool() const;

	private:

		NodeGraphVariant mNodeGraphVariant;

	};
}

template<>
struct std::hash<FLY_NAMESPACE::NodeGraphView>
{
	size_t operator()(const FLY_NAMESPACE::NodeGraphView& aNodeGraphView) const
	{
		return reinterpret_cast<size_t>(&aNodeGraphView.GetNodeGraph());
	}
};