#include "NodeView.h"
#include "../ScriptNodeGraph.h"
#include "../Global/ScriptGlobal.h"
#include "../Node/NodeTypeManager.h"

namespace SCR
{

	NodeView::NodeView(const NodeID aNodeID, const NodeGraph& aNodeGraph)
		: myNodeID(aNodeID)
		, myNodeGraph(&aNodeGraph)
	{
	}

	const std::string& NodeView::GetNodeTypeName() const
	{
		return GetNodeType().name;
	}

	std::string NodeView::GetShortName() const
	{
		return Global::GetNodeTypeManager().GetShortName(GetNode().typeID);
	}

	Vec2 NodeView::GetPosition() const
	{
		return GetNode().position;
	}

	bool NodeView::IsDestroyed() const
	{
		return GetNode().isDestroyed;
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
		return myNodeGraph->myNodeManager->myNodes.at(myNodeID);
	}

	const NodeType& NodeView::GetNodeType() const
	{
		const Node& node = GetNode();
		return Global::GetNodeTypeManager().GetNodeType(node.typeID);
	}

	std::vector<PinView> NodeView::GetPinViews(const eFlowType aFlowType) const
	{
		const Node& node = GetNode();
		std::vector<PinView> pinViews;

		const std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.inputPins, node.outputPins);
		pinViews.reserve(pinIDs.size());

		for (const PinID pinID : pinIDs)
		{
			pinViews.emplace_back(PinView(pinID, *myNodeGraph));
		}

		return pinViews;
	}

	NodeID NodeView::GetID() const
	{
		return myNodeID;
	}

	eNodeTrait NodeView::GetTraits() const
	{
		return GetNodeType().nodeRecipe.traits;
	}

	EventID NodeView::GetEventID() const
	{
		return GetNodeType().nodeRecipe.eventID;
	}

	const NodeGraph& NodeView::GetNodeGraph() const
	{
		return *myNodeGraph;
	}

	bool NodeView::operator==(const NodeView& aOther) const
	{
		return myNodeGraph == aOther.myNodeGraph && myNodeID == aOther.myNodeID;
	}

	bool NodeView::operator!=(const NodeView& aOther) const
	{
		return !(*this == aOther);
	}
}
