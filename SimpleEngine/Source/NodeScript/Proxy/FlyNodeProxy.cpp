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
		return GetNode().GetPosition();
	}

	bool NodeProxy::IsDestroyed() const
	{
		return GetNode().IsDestroyed();
	}

	std::vector<PinProxy> NodeProxy::GetInputPins() const
	{
		return GetPins(eIODirection::Input);
	}

	std::vector<PinProxy> NodeProxy::GetOutputPins() const
	{
		return GetPins(eIODirection::Output);
	}

	std::vector<PinProxy> NodeProxy::GetSplitInputPins() const
	{
		return GetSplitPins(eIODirection::Input);
	}

	std::vector<PinProxy> NodeProxy::GetSplitOutputPins() const
	{
		return GetSplitPins(eIODirection::Output);
	}

	const Node& NodeProxy::GetNode() const
	{
		return GetNodeGraph().GetNode(mNodeID);
	}

	NodeID NodeProxy::GetID() const
	{
		return mNodeID;
	}

	eNodeTrait NodeProxy::GetTraits() const
	{
		return Internal::GetNodeType(GetNode().GetTypeID()).GetTraits();
	}

	bool NodeProxy::IsAccessor() const
	{
		return HasFlag(GetTraits(), eNodeTrait::Accessor);
	}

	EventID NodeProxy::GetEventID() const
	{
		return Internal::GetNodeType(GetNode().GetTypeID()).GetEventID();
	}

	bool NodeProxy::HasAnyConnectedLinks() const
	{
		const Node& node = GetNode();

		auto hasConnectedLink = [](const std::vector<PinID>& aPinIDs, const NodeGraph& aNodeGraph) -> bool
			{
				for (const PinID pinID : aPinIDs)
				{
					const Pin& pin = aNodeGraph.GetPin(pinID);
					if (!pin.GetConnectedPinIDs().empty())
					{
						return true;
					}
				}

				return false;
			};

		return hasConnectedLink(node.GetInputPins(), GetNodeGraph()) || hasConnectedLink(node.GetOutputPins(), GetNodeGraph());
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
		return NodeTypeProxy(GetNode().GetTypeID());
	}

	void NodeProxy::Destroy(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyNode(GetNodeGraph(), GetID(), aCommandTracker);
	}

	void NodeProxy::DestroyConnectedLinks(CommandTracker* const aCommandTracker)
	{
		std::vector<LinkID> linkIDs = Internal::GetLinkIDsByNode(NodeGraphProxy(mNodeGraphVariant).GetNodeGraph(), mNodeID);
		Internal::DestroyLinks(std::span(linkIDs), NodeGraphProxy(mNodeGraphVariant).GetNodeGraph(), aCommandTracker);
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

	std::vector<PinProxy> NodeProxy::GetPins(const eIODirection aIODirection) const
	{
		const Node& node = GetNode();
		return GetPins(SelectByIODirection(aIODirection, node.GetInputPins(), node.GetOutputPins()));
	}

	std::vector<PinProxy> NodeProxy::GetSplitPins(const eIODirection aIODirection) const
	{
		const Node& node = GetNode();
		return GetPins(SelectByIODirection(aIODirection, node.GetSplitInputPins(), node.GetSplitOutputPins()));
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
