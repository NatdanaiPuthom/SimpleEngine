#include "NodeView.hpp"
#include "../FlyNodeGraph.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Node/NodeTypeManager.hpp"

namespace FLY_NAMESPACE
{

	NodeView::NodeView(const NodeID aNodeID, const NodeGraph& aNodeGraph)
		: mNodeID(aNodeID)
		, mNodeGraph(&aNodeGraph)
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

	std::vector<PinView> NodeView::GetInputPins() const
	{
		return GetPinViews(eFlowType::Input);
	}

	std::vector<PinView> NodeView::GetOutputPins() const
	{
		return GetPinViews(eFlowType::Output);
	}

	const Node& NodeView::GetNode() const
	{
		return mNodeGraph->mNodes.at(mNodeID);
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
			pinViews.emplace_back(PinView(pinID, *mNodeGraph));
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

	const NodeGraph& NodeView::GetNodeGraph() const
	{
		return *mNodeGraph;
	}

	bool NodeView::operator==(const NodeView& aOther) const
	{
		return mNodeGraph == aOther.mNodeGraph && mNodeID == aOther.mNodeID;
	}

	bool NodeView::operator!=(const NodeView& aOther) const
	{
		return !(*this == aOther);
	}
}
