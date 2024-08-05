#include "FlyNodeView.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "FlyNodeGraphView.hpp"
#include "Fly.hpp"

namespace FLY_NAMESPACE
{

	NodeView::NodeView(const NodeID aNodeID, const NodeGraphView& aNodeGraphView)
		: mNodeGraphVariant(aNodeGraphView.GetVariant())
		, mNodeID(aNodeID)
	{
	}

	const std::string& NodeView::GetNodeTypeName() const
	{
		return GetNodeType().mName;
	}

	std::string NodeView::GetShortName() const
	{
		return Global::GetNodeTypeManager().GetShortName(GetNode().mTypeID);
	}

	Vec2 NodeView::GetPosition() const
	{
		return GetNode().mPosition;
	}

	bool NodeView::IsDestroyed() const
	{
		return GetNode().mIsDestroyed;
	}

	std::vector<PinView> NodeView::GetInputPinViews() const
	{
		return GetPinViews(eFlowType::Input);
	}

	std::vector<PinView> NodeView::GetOutputPinViews() const
	{
		return GetPinViews(eFlowType::Output);
	}

	const Node& NodeView::GetNode() const
	{
		return GetNodeGraph().mNodes.at(mNodeID);
	}

	const NodeType& NodeView::GetNodeType() const
	{
		const Node& node = GetNode();
		return Global::GetNodeTypeManager().GetNodeType(node.mTypeID);
	}

	std::vector<PinView> NodeView::GetPinViews(const eFlowType aFlowType) const
	{
		const Node& node = GetNode();
		std::vector<PinView> pinViews;

		const std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
		pinViews.reserve(pinIDs.size());

		for (const PinID pinID : pinIDs)
		{
			pinViews.emplace_back(PinView(pinID, NodeGraphView(mNodeGraphVariant)));
		}

		return pinViews;
	}

	NodeID NodeView::GetID() const
	{
		return mNodeID;
	}

	eNodeTrait NodeView::GetTraits() const
	{
		return GetNodeType().mNodeRecipe.mTraits;
	}

	EventID NodeView::GetEventID() const
	{
		return GetNodeType().mNodeRecipe.mEventID;
	}

	void NodeView::Destroy(CommandTracker* const aCommandTracker)
	{
		Fly::DestroyNode(*this, NodeGraphView(mNodeGraphVariant), aCommandTracker);
	}

	void NodeView::DestroyConnectedLinks(CommandTracker* const aCommandTracker)
	{
		Fly::DestroyLinksByNode(*this, NodeGraphView(mNodeGraphVariant), aCommandTracker);
	}

	const NodeGraph& NodeView::GetNodeGraph() const
	{
		return NodeGraphView(mNodeGraphVariant).GetNodeGraph();
	}

	bool operator==(const NodeView& a, const NodeView& b)
	{
		return NodeGraphView(a.mNodeGraphVariant) == NodeGraphView(b.mNodeGraphVariant) && a.mNodeID == b.mNodeID;
	}

	NodeView::operator bool() const
	{
		return NodeGraphView(mNodeGraphVariant) && mNodeID != InvalidID<NodeID>();
	}
}
