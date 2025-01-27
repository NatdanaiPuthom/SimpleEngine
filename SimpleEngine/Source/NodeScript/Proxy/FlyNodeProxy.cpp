#include "FlyNodeProxy.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "FlyNodeGraphProxy.hpp"
#include "Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	NodeProxy::NodeProxy(const NodeID aNodeID, const NodeGraphProxy& aNodeGraphProxy)
		: mNodeGraphVariant(aNodeGraphProxy.GetVariant())
		, mNodeID(aNodeID)
	{
	}

	NodeProxy::NodeProxy(const NodeID aNodeID, const NodeGraphVariantHandle& aNodeGraphVariantHandle)
		: mNodeGraphVariant(aNodeGraphVariantHandle)
		, mNodeID(aNodeID)
	{
	}

	Vec2 NodeProxy::GetPosition() const
	{
		return GetNode().mPosition;
	}

	bool NodeProxy::IsDestroyed() const
	{
		return GetNode().mIsDestroyed;
	}

	std::vector<PinProxy> NodeProxy::GetInputPins() const
	{
		return GetPins(eFlowType::Input);
	}

	std::vector<PinProxy> NodeProxy::GetOutputPins() const
	{
		return GetPins(eFlowType::Output);
	}

	std::vector<PinProxy> NodeProxy::GetSplitInputPins() const
	{
		return GetSplitPins(eFlowType::Input);
	}

	std::vector<PinProxy> NodeProxy::GetSplitOutputPins() const
	{
		return GetSplitPins(eFlowType::Output);
	}

	const Node& NodeProxy::GetNode() const
	{
		return GetNodeGraph().mNodes.at(mNodeID);
	}

	NodeID NodeProxy::GetID() const
	{
		return mNodeID;
	}

	eNodeTrait NodeProxy::GetTraits() const
	{
		return Internal::GetNodeType(GetNode().mTypeID).mNodeRecipe.mTraits;
	}

	bool NodeProxy::IsAccessor() const
	{
		return HasFlag(GetTraits(), eNodeTrait::Accessor);
	}

	EventID NodeProxy::GetEventID() const
	{
		return Internal::GetNodeType(GetNode().mTypeID).mNodeRecipe.mEventID;
	}

	bool NodeProxy::HasAnyConnectedLinks() const
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

	bool NodeProxy::IsReplacable() const
	{
		return Internal::IsNodeReplacable(NodeGraphProxy(mNodeGraphVariant).GetNodeGraph(), GetID());
	}

	/*VariableProxy NodeProxy::GetVariableProxy() const
	{
		const VariableRef variableRef = Internal::GetVariableRefByNodeRef(GlobalNodeRef(mNodeID, GetNodeGraph()));
		return VariableProxy(variableRef.GetVarID(), ClassProxy(variableRef.GetClass()));
	}*/

	NodeTypeProxy NodeProxy::GetNodeType() const
	{
		return NodeTypeProxy(GetNode().mTypeID);
	}

	void NodeProxy::Destroy(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyNode(GetNodeGraph(), GetID(), aCommandTracker);
	}

	void NodeProxy::DestroyConnectedLinks(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyLinks(Internal::GetLinkIDsByNode(NodeGraphProxy(mNodeGraphVariant).GetNodeGraph(), GetID()), NodeGraphProxy(mNodeGraphVariant).GetNodeGraph(), aCommandTracker);
	}

	void NodeProxy::SetPosition(const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		Internal::SetNodePosition(GetID(), aPosition, GetNodeGraph(), aCommandTracker);
	}

	NodeGraph& NodeProxy::GetNodeGraph() const
	{
		return NodeGraphProxy(mNodeGraphVariant).GetNodeGraph();
	}

	std::vector<PinProxy> NodeProxy::GetPins(const std::vector<PinID>& aPinIDs) const
	{
		std::vector<PinProxy> pinProxys;

		pinProxys.reserve(aPinIDs.size());

		for (const PinID pinID : aPinIDs)
		{
			pinProxys.emplace_back(PinProxy(pinID, NodeGraphProxy(mNodeGraphVariant)));
		}

		return pinProxys;
	}

	std::vector<PinProxy> NodeProxy::GetPins(const eFlowType aFlowType) const
	{
		const Node& node = GetNode();
		return GetPins(SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins));
	}

	std::vector<PinProxy> NodeProxy::GetSplitPins(const eFlowType aFlowType) const
	{
		const Node& node = GetNode();
		return GetPins(SelectByFlowType(aFlowType, node.mSplitInputPins, node.mSplitOutputPins));
	}

	bool operator==(const NodeProxy& a, const NodeProxy& b)
	{
		return NodeGraphProxy(a.mNodeGraphVariant) == NodeGraphProxy(b.mNodeGraphVariant) && a.mNodeID == b.mNodeID;
	}

	NodeProxy::operator bool() const
	{
		return NodeGraphProxy(mNodeGraphVariant) && mNodeID != InvalidID<NodeID>();
	}
}
