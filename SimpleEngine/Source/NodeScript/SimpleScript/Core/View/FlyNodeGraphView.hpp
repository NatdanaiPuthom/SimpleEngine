#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeView.hpp"
#include "FlyPinView.hpp"
#include "FlyLinkView.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"

namespace FLY_NAMESPACE
{
	
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

		std::vector<NodeView> GetNodes(bool aIncludeDestroyed = false) const;
		std::vector<PinView> GetPins(bool aIncludeDestroyed = false) const;

		NodeGraph& GetNodeGraph();
		const NodeGraph& GetNodeGraph() const;

		eNodeGraphType GetType() const;

		NodeView CreateNode(const NodeTypeView& aNodeTypeView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeView CreateNode(std::string_view aName, bool& aSuccess, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
		NodeView CreateNodeAutoLink(NodeTypeView aNodeTypeView, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeView CreateGetterNode(const ClassView& aClassView, VariableView aVariableView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeView CreateSetterNode(const ClassView& aClassView, VariableView aVariableView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, CommandTracker* aCommandTracker);
		LinkView TryCreateLink(PinView aPinView1, PinView aPinView2, CommandTracker* aCommandTracker);

		const NodeGraphVariant& GetVariant() const;

		std::variant<NodeGraph*, EventGraph*> GetAs();

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