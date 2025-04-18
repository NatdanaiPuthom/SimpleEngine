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

	static auto FindFirstNonDestroyed(const auto& aList)
	{
		auto it = std::find_if(begin(aList), end(aList), [](const auto& aElement) -> bool
			{
				return !aElement.mIsDestroyed;
			});
		return std::distance(begin(aList), it);
	}

	NodeProxyIteratorService NodeGraphProxy::IterateNodes(Predicate<NodeProxy> aFilterPredicate) const
	{
		return NodeProxyIteratorService(mNodeGraphVariant, NodeID{ static_cast<NodeID::value_type>(GetNodeGraph().GetNodeCount()) }, aFilterPredicate);
	}

	NodeProxyIteratorService NodeGraphProxy::IterateNodes(const bool aIncludeDestroyed) const
	{
		Predicate<NodeProxy> filterPredicate = aIncludeDestroyed
			? Predicate<NodeProxy>()
			: [](const NodeProxy& aNodeProxy) -> bool
			{
				return !aNodeProxy.IsDestroyed();
			};
		return NodeProxyIteratorService(mNodeGraphVariant, NodeID{ static_cast<NodeID::value_type>(GetNodeGraph().GetNodeCount()) }, filterPredicate);
	}

	LinkProxyIteratorService NodeGraphProxy::IterateLinks(const bool aIncludeDestroyed) const
	{
		Predicate<LinkProxy> filterPredicate = aIncludeDestroyed
			? Predicate<LinkProxy>()
			: [](const LinkProxy& aLinkProxy) -> bool
			{
				return !aLinkProxy.IsDestroyed();
			};
		return LinkProxyIteratorService(mNodeGraphVariant, LinkID{ static_cast<LinkID::value_type>(GetNodeGraph().GetLinks().size())}, filterPredicate);
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

	template<IsPredicate<const Pin&> Predicate>
	std::vector<PinProxy> GetPinsFiltered(Predicate&& aPredicate, const NodeGraphProxy& aNodeGraphProxy)
	{
		std::vector<PinProxy> pinProxys;
		const NodeGraph& nodeGraph = aNodeGraphProxy.GetNodeGraph();
		pinProxys.reserve(nodeGraph.GetPinCount());

		for (PinID pinID{ 0 }; pinID < nodeGraph.GetPinCount(); ++pinID)
		{
			const Pin& pin = nodeGraph.GetPin(pinID);
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
				return aPin.GetConnectedPinIDs().empty() && Internal::GetPinType(aPin.GetTypeID()).GetIODirection() == eIODirection::Input;
			},
			*this
		);
	}

	std::vector<PinProxy> NodeGraphProxy::GetNonConnectedOutputPins() const
	{
		return GetPinsFiltered([](const Pin& aPin)-> bool
			{
				return aPin.GetConnectedPinIDs().empty() && Internal::GetPinType(aPin.GetTypeID()).GetIODirection() == eIODirection::Output;
			},
			*this
		);
	}

	std::vector<PinProxy> NodeGraphProxy::GetNonConnectedPinsByIODirection(const eIODirection aIODirection) const
	{
		return SelectByIODirection(aIODirection, GetNonConnectedInputPins(), GetNonConnectedOutputPins());
	}

	std::vector<PinProxy> NodeGraphProxy::GetNonConnectedPinsByIODirectionAndDataType(const eIODirection aIODirection, const GenericDataTypeProxy aDataTypeProxy) const
	{
		return GetPinsFiltered(
			[aIODirection, dataTypeID = aDataTypeProxy.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Internal::GetPinTypeManager().GetPinType(aPin.GetTypeID());
				return aPin.GetConnectedPinIDs().empty() && pinType.GetIODirection() == aIODirection && pinType.GetDataTypeID() == dataTypeID;
			},
			*this
		);
	}

	std::vector<PinProxy> NodeGraphProxy::GetNonConnectedPinsByIODirectionAndRelatedDataTypes(const eIODirection aIODirection, const GenericDataTypeProxy aDataTypeProxy) const
	{
		return GetPinsFiltered(
			[aIODirection, dataTypeID = aDataTypeProxy.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Internal::GetPinTypeManager().GetPinType(aPin.GetTypeID());
				const bool a = aPin.GetConnectedPinIDs().empty() && pinType.GetIODirection() == aIODirection;

				return a && Internal::AreDataTypesLinkable(SelectByIODirection(aIODirection, dataTypeID, pinType.GetDataTypeID()), pinType.GetDataTypeID());
			},
			*this
		);
	}

	NodeProxy NodeGraphProxy::CreateNode(const NodeTypeProxy& aNodeTypeProxy, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateNode(mNodeGraphVariant, aNodeTypeProxy.GetID(), aPosition, aCommandTracker);
		return NodeProxy(nodeID, *this);
	}

	NodeProxy NodeGraphProxy::CreateNode(const std::string_view aName, const Vec2 aPosition, CommandTracker* const aCommandTracker, const bool aCreateIfNameNotFound)
	{
		const std::optional<NodeID> nodeID = Internal::CreateNode(mNodeGraphVariant, aName, aPosition, aCreateIfNameNotFound, aCommandTracker);
		if (nodeID.has_value())
		{
			return NodeProxy(nodeID.value(), *this);
		}
		else
		{
			return NodeProxy(InvalidID<NodeID>(), *this);
		}
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

	void NodeGraphProxy::DestroySelection(const std::span<NodeID> aNodeIDs, const std::span<LinkID> aLinkIDs, CommandTracker* const aCommandTracker)
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
