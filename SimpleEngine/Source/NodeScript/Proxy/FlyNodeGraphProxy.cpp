#include "FlyNodeGraphProxy.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "FlyFunctionProxy.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "FlyVariableProxy.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	NodeGraphProxy::NodeGraphProxy(const FunctionProxy& aFunctionProxy)
		: mNodeGraphVariant(FunctionIDWrapper(aFunctionProxy.GetID()))
	{
	}

	NodeGraphProxy::NodeGraphProxy(EventGraph& aEventGraph)
		: mNodeGraphVariant(&aEventGraph)
	{
	}

	NodeGraphProxy::NodeGraphProxy(NodeGraphVariantHandle&& aNodeGraphVariant)
		: mNodeGraphVariant(std::forward<NodeGraphVariantHandle>(aNodeGraphVariant))
	{
	}

	NodeGraphProxy::NodeGraphProxy(const NodeGraphVariantHandle& aNodeGraphVariant)
		: mNodeGraphVariant(aNodeGraphVariant)
	{
	}

	std::vector<NodeProxy> NodeGraphProxy::GetNodeProxys(const bool aIncludeDestroyed) const
	{
		const std::vector<Node>& nodes = GetNodeGraph().mNodes;

		std::vector<NodeProxy> nodeProxys;
		nodeProxys.reserve(nodes.size());

		for (NodeID nodeID{ 0 }; nodeID < nodes.size(); ++nodeID)
		{
			NodeProxy nodeProxy(nodeID, *this);
			if (!aIncludeDestroyed && nodeProxy.IsDestroyed())
			{
				continue;
			}
			nodeProxys.push_back(nodeProxy);
		}

		return nodeProxys;
	}

	std::vector<PinProxy> NodeGraphProxy::GetPinProxys(const bool aIncludeDestroyed) const
	{
		const std::vector<Pin>& pins = GetNodeGraph().mPins;

		std::vector<PinProxy> pinProxys;
		pinProxys.reserve(pins.size());

		for (PinID pinID{ 0 }; pinID < pins.size(); ++pinID)
		{
			PinProxy pinProxy(pinID, *this);
			NodeProxy nodeProxy(pinProxy.GetNodeID(), *this);
			if (!aIncludeDestroyed && nodeProxy.IsDestroyed())
			{
				continue;
			}
			pinProxys.push_back(pinProxy);
		}

		return pinProxys;
	}

	NodeGraph& NodeGraphProxy::GetNodeGraph()
	{
		return Internal::GetNodeGraph(mNodeGraphVariant);
	}

	const NodeGraph& NodeGraphProxy::GetNodeGraph() const
	{
		return Internal::GetNodeGraph(mNodeGraphVariant);
	}

	eNodeGraphType NodeGraphProxy::GetType() const
	{
		return std::visit(Visitor{
			[](EventGraph*) -> eNodeGraphType
			{
				return eNodeGraphType::EventGraph;
			},
			[](FunctionIDWrapper) -> eNodeGraphType
			{
				return eNodeGraphType::Function;
			}
			},
			mNodeGraphVariant);
	}

	template<Predicate<const Pin&> Predicate>
	std::vector<PinProxy> GetPinsFiltered(Predicate&& aPredicate, const NodeGraphProxy& aNodeGraphProxy)
	{
		std::vector<PinProxy> pinProxys;
		const NodeGraph& nodeGraph = aNodeGraphProxy.GetNodeGraph();
		pinProxys.reserve(nodeGraph.mPins.size());

		for (PinID pinID{ 0 }; pinID < nodeGraph.mPins.size(); ++pinID)
		{
			const Pin& pin = nodeGraph.mPins[pinID];
			if (aPredicate(pin))
			{
				pinProxys.push_back(PinProxy(pinID, aNodeGraphProxy));
			}
		}

		return pinProxys;
	}

	std::vector<PinProxy> NodeGraphProxy::GetNonConnectedInputPins() const
	{
		return GetPinsFiltered([](const Pin& aPin)-> bool
			{
				return aPin.mConnectedPinIDs.empty() && Internal::GetPinType(aPin.mTypeID).mFlowType == eFlowType::Input;
			},
			*this
		);
	}

	std::vector<PinProxy> NodeGraphProxy::GetNonConnectedOutputPins() const
	{
		return GetPinsFiltered([](const Pin& aPin)-> bool
			{
				return aPin.mConnectedPinIDs.empty() && Internal::GetPinType(aPin.mTypeID).mFlowType == eFlowType::Output;
			},
			*this
		);
	}

	std::vector<PinProxy> NodeGraphProxy::GetNonConnectedPinsByFlowType(const eFlowType aFlowType) const
	{
		switch (aFlowType)
		{
		case eFlowType::Input:
			return GetNonConnectedInputPins();
			break;
		case eFlowType::Output:
			return GetNonConnectedOutputPins();
			break;
		default:
			break;
		}
		return std::vector<PinProxy>();
	}

	std::vector<PinProxy> NodeGraphProxy::GetNonConnectedPinsByFlowTypeAndDataType(const eFlowType aFlowType, const GenericDataTypeProxy aDataTypeProxy) const
	{
		return GetPinsFiltered(
			[aFlowType, dataTypeID = aDataTypeProxy.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Internal::GetPinTypeManager().GetPinType(aPin.mTypeID);
				return aPin.mConnectedPinIDs.empty() && pinType.mFlowType == aFlowType && pinType.mGenericDataTypeID == dataTypeID;
			},
			*this
		);
	}

	std::vector<PinProxy> NodeGraphProxy::GetNonConnectedPinsByFlowTypeAndRelatedDataTypes(const eFlowType aFlowType, const GenericDataTypeProxy aDataTypeProxy) const
	{
		return GetPinsFiltered(
			[aFlowType, dataTypeID = aDataTypeProxy.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Internal::GetPinTypeManager().GetPinType(aPin.mTypeID);
				const bool a = aPin.mConnectedPinIDs.empty() && pinType.mFlowType == aFlowType;

				return a && Internal::AreDataTypesLinkable(SelectByFlowType(aFlowType, dataTypeID, pinType.mGenericDataTypeID), pinType.mGenericDataTypeID);
			},
			*this
		);
	}


	std::vector<LinkProxy> NodeGraphProxy::GetLinks(const bool aIncludeDestroyed) const
	{
		const std::vector<Link>& links = GetNodeGraph().mLinks;
		std::vector<LinkProxy> linkProxys;
		linkProxys.reserve(links.size());

		for (LinkID linkID{ 0 }; linkID < links.size(); linkID++)
		{
			const Link& link = links[linkID];
			if (!aIncludeDestroyed && link.mIsDestroyed)
			{
				continue;
			}

			linkProxys.emplace_back(linkID, *this);
		}

		return linkProxys;
	}

	NodeProxy NodeGraphProxy::CreateNode(const NodeTypeProxy& aNodeTypeProxy, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateNode(mNodeGraphVariant, aNodeTypeProxy.GetID(), aPosition, aCommandTracker);
		return NodeProxy(nodeID, *this);
	}

	NodeProxy NodeGraphProxy::CreateNode(const std::string_view aName, bool& aSuccess, const Vec2 aPosition, CommandTracker* const aCommandTracker, const bool aCreateIfNameNotFound)
	{
		const NodeID nodeID = Internal::CreateNode(mNodeGraphVariant, aName, aSuccess, aPosition, aCreateIfNameNotFound, aCommandTracker);
		return NodeProxy(nodeID, *this);
	}

	NodeProxy NodeGraphProxy::CreateNodeAutoLink(const NodeTypeProxy aNodeTypeProxy, const PinID aConnection, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateNodeAutoLink(mNodeGraphVariant, aNodeTypeProxy.GetID(), aConnection, aPosition, aCommandTracker);
		return NodeProxy(nodeID, *this);
	}

	/*NodeProxy NodeGraphProxy::CreateGetterNode(const VariableProxy aVariableProxy, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateGetterNode(GetNodeGraph(), aVariableProxy.GetID(), aVariableProxy.GetClass(), aVariableProxy.GetDataType().GetID(), aPosition, aCommandTracker);
		return NodeProxy(nodeID, *this);
	}

	NodeProxy NodeGraphProxy::CreateSetterNode(const VariableProxy aVariableProxy, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateSetterNode(GetNodeGraph(), aVariableProxy.GetID(), aVariableProxy.GetClass(), aVariableProxy.GetDataType().GetID(), aPosition, aCommandTracker);
		return NodeProxy(nodeID, *this);
	}*/

	void NodeGraphProxy::DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, CommandTracker* const aCommandTracker)
	{
		Internal::DestroySelection(aNodeIDs, aLinkIDs, GetNodeGraph(), aCommandTracker);
	}

	LinkProxy NodeGraphProxy::TryCreateLink(const PinProxy aPinProxy1, const PinProxy aPinProxy2, CommandTracker* const aCommandTracker)
	{
		const LinkID linkID = Internal::TryCreateLink(GetNodeGraph(), aPinProxy1.GetID(), aPinProxy2.GetID(), aCommandTracker);
		return LinkProxy(linkID, *this);
	}

	void NodeGraphProxy::CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, CommandTracker* const aCommandTracker)
	{
		Internal::CommitNodeDrag(aNodeDragData, GetNodeGraph(), aCommandTracker);
	}

	void NodeGraphProxy::ReplaceTemplateNode(const NodeProxy aReplaceNodeProxy, const DataTypeProxy aDataTypeProxy, CommandTracker* const aCommandTracker)
	{
		Internal::ReplaceNode(GetNodeGraph(), aReplaceNodeProxy.GetID(), aDataTypeProxy.GetID(), aCommandTracker);
	}

	void NodeGraphProxy::ReplaceTemplateNode(const PinProxy aReplacePinProxy, const DataTypeProxy aDataTypeProxy, CommandTracker* const aCommandTracker)
	{
		Internal::ReplaceTemplateNode(GetNodeGraph(), aReplacePinProxy.GetNodeID(), aDataTypeProxy.GetID(), aCommandTracker);
	}

	const NodeGraphVariantHandle& NodeGraphProxy::GetVariant() const
	{
		return mNodeGraphVariant;
	}

	bool operator==(const NodeGraphProxy& a, const NodeGraphProxy& b)
	{
		return a.mNodeGraphVariant == b.mNodeGraphVariant;
	}

	NodeGraphProxy::operator bool() const
	{
		return std::visit(Visitor{
			[](const EventGraph* anEventGraph) -> bool
			{
				return anEventGraph != nullptr;
			},
			[](FunctionIDWrapper aFunctionIDWrapper) -> bool
			{
				return aFunctionIDWrapper.mID != InvalidID<FunctionID>();
			}
			}, mNodeGraphVariant
		);
	}
}
