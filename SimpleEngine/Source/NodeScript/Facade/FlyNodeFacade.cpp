#include "FlyNodeFacade.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "FlyNodeGraphFacade.hpp"
#include "Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	NodeFacade::NodeFacade(const NodeID aNodeID, const NodeGraphFacade& aNodeGraphFacade)
		: mNodeGraphVariant(aNodeGraphFacade.GetVariant())
		, mNodeID(aNodeID)
	{
	}

	Vec2 NodeFacade::GetPosition() const
	{
		return GetNode().mPosition;
	}

	bool NodeFacade::IsDestroyed() const
	{
		return GetNode().mIsDestroyed;
	}

	std::vector<PinFacade> NodeFacade::GetInputPinFacades() const
	{
		return GetPinFacades(eFlowType::Input);
	}

	std::vector<PinFacade> NodeFacade::GetOutputPinFacades() const
	{
		return GetPinFacades(eFlowType::Output);
	}

	std::vector<PinFacade> NodeFacade::GetSplitInputPinFacades() const
	{
		return GetSplitPinFacades(eFlowType::Input);
	}

	std::vector<PinFacade> NodeFacade::GetSplitOutputPinFacades() const
	{
		return GetSplitPinFacades(eFlowType::Output);
	}

	const Node& NodeFacade::GetNode() const
	{
		return GetNodeGraph().mNodes.at(mNodeID);
	}

	const NodeType& NodeFacade::GetNodeType() const
	{
		const Node& node = GetNode();
		return Internal::GetNodeTypeManager().GetNodeType(node.mTypeID);
	}

	NodeID NodeFacade::GetID() const
	{
		return mNodeID;
	}

	eNodeTrait NodeFacade::GetTraits() const
	{
		return GetNodeType().mNodeRecipe.mTraits;
	}

	bool NodeFacade::IsAccessor() const
	{
		return HasFlag(GetTraits(), eNodeTrait::Accessor);
	}

	EventID NodeFacade::GetEventID() const
	{
		return GetNodeType().mNodeRecipe.mEventID;
	}

	bool NodeFacade::HasAnyConnectedLinks() const
	{
		const Node& node = GetNode();

		auto hasConnectedLink = [](const std::vector<PinID>& aPinIDs, const NodeGraph& aNodeGraph) -> bool
			{
				for (const PinID pinID : aPinIDs)
				{
					const Pin& pin = aNodeGraph.mPins.at(pinID);
					if (!pin.mConnectedPinIDs.empty())
					{
						return true;
					}
				}

				return false;
			};

		return hasConnectedLink(node.mInputPins, GetNodeGraph()) || hasConnectedLink(node.mOutputPins, GetNodeGraph());
	}

	bool NodeFacade::IsReplacable() const
	{
		return Internal::IsNodeReplacable(NodeGraphFacade(mNodeGraphVariant).GetNodeGraph(), GetID());
	}

	/*VariableFacade NodeFacade::GetVariableFacade() const
	{
		const VariableRef variableRef = Internal::GetVariableRefByNodeRef(GlobalNodeRef(mNodeID, GetNodeGraph()));
		return VariableFacade(variableRef.GetVarID(), ClassFacade(variableRef.GetClass()));
	}*/

	NodeTypeFacade NodeFacade::GetNodeTypeFacade() const
	{
		return NodeTypeFacade(GetNode().mTypeID);
	}

	void NodeFacade::Destroy(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyNode(GetNodeGraph(), GetID(), aCommandTracker);
	}

	void NodeFacade::DestroyConnectedLinks(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyLinks(Internal::GetLinkIDsByNode(NodeGraphFacade(mNodeGraphVariant).GetNodeGraph(), GetID()), NodeGraphFacade(mNodeGraphVariant).GetNodeGraph(), aCommandTracker);
	}

	void NodeFacade::SetPosition(const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		Internal::SetNodePosition(GetID(), aPosition, GetNodeGraph(), aCommandTracker);
	}

	NodeGraph& NodeFacade::GetNodeGraph() const
	{
		return NodeGraphFacade(mNodeGraphVariant).GetNodeGraph();
	}

	std::vector<PinFacade> NodeFacade::GetPinFacades(const std::vector<PinID>& aPinIDs) const
	{
		std::vector<PinFacade> pinFacades;

		pinFacades.reserve(aPinIDs.size());

		for (const PinID pinID : aPinIDs)
		{
			pinFacades.emplace_back(PinFacade(pinID, NodeGraphFacade(mNodeGraphVariant)));
		}

		return pinFacades;
	}

	std::vector<PinFacade> NodeFacade::GetPinFacades(const eFlowType aFlowType) const
	{
		const Node& node = GetNode();
		return GetPinFacades(SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins));
	}

	std::vector<PinFacade> NodeFacade::GetSplitPinFacades(const eFlowType aFlowType) const
	{
		const Node& node = GetNode();
		return GetPinFacades(SelectByFlowType(aFlowType, node.mSplitInputPins, node.mSplitOutputPins));
	}

	bool operator==(const NodeFacade& a, const NodeFacade& b)
	{
		return NodeGraphFacade(a.mNodeGraphVariant) == NodeGraphFacade(b.mNodeGraphVariant) && a.mNodeID == b.mNodeID;
	}

	NodeFacade::operator bool() const
	{
		return NodeGraphFacade(mNodeGraphVariant) && mNodeID != InvalidID<NodeID>();
	}
}
