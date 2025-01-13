#include "FlyNodeGraphFacade.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "FlyFunctionFacade.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "FlyVariableFacade.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	NodeGraphFacade::NodeGraphFacade(const FunctionFacade& aFunctionFacade)
		: mNodeGraphVariant(FunctionIDWrapper(aFunctionFacade.GetID()))
	{
	}

	NodeGraphFacade::NodeGraphFacade(EventGraph& aEventGraph)
		: mNodeGraphVariant(&aEventGraph)
	{
	}

	NodeGraphFacade::NodeGraphFacade(NodeGraphVariantHandle&& aNodeGraphVariant)
		: mNodeGraphVariant(std::forward<NodeGraphVariantHandle>(aNodeGraphVariant))
	{
	}

	NodeGraphFacade::NodeGraphFacade(const NodeGraphVariantHandle& aNodeGraphVariant)
		: mNodeGraphVariant(aNodeGraphVariant)
	{
	}

	std::vector<NodeFacade> NodeGraphFacade::GetNodeFacades(const bool aIncludeDestroyed) const
	{
		const std::vector<Node>& nodes = GetNodeGraph().mNodes;

		std::vector<NodeFacade> nodeFacades;
		nodeFacades.reserve(nodes.size());

		for (NodeID nodeID{ 0 }; nodeID < nodes.size(); ++nodeID)
		{
			NodeFacade nodeFacade(nodeID, *this);
			if (!aIncludeDestroyed && nodeFacade.IsDestroyed())
			{
				continue;
			}
			nodeFacades.push_back(nodeFacade);
		}

		return nodeFacades;
	}

	std::vector<PinFacade> NodeGraphFacade::GetPinFacades(const bool aIncludeDestroyed) const
	{
		const std::vector<Pin>& pins = GetNodeGraph().mPins;

		std::vector<PinFacade> pinFacades;
		pinFacades.reserve(pins.size());

		for (PinID pinID{ 0 }; pinID < pins.size(); ++pinID)
		{
			PinFacade pinFacade(pinID, *this);
			NodeFacade nodeFacade(pinFacade.GetNodeID(), *this);
			if (!aIncludeDestroyed && nodeFacade.IsDestroyed())
			{
				continue;
			}
			pinFacades.push_back(pinFacade);
		}

		return pinFacades;
	}

	NodeGraph& NodeGraphFacade::GetNodeGraph()
	{
		return Internal::GetNodeGraph(mNodeGraphVariant);
	}

	const NodeGraph& NodeGraphFacade::GetNodeGraph() const
	{
		return Internal::GetNodeGraph(mNodeGraphVariant);
	}

	eNodeGraphType NodeGraphFacade::GetType() const
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
	std::vector<PinFacade> GetPinFacadesFiltered(Predicate&& aPredicate, const NodeGraphFacade& aNodeGraphFacade)
	{
		std::vector<PinFacade> pinFacades;
		const NodeGraph& nodeGraph = aNodeGraphFacade.GetNodeGraph();
		pinFacades.reserve(nodeGraph.mPins.size());

		for (PinID pinID{ 0 }; pinID < nodeGraph.mPins.size(); ++pinID)
		{
			const Pin& pin = nodeGraph.mPins[pinID];
			if (aPredicate(pin))
			{
				pinFacades.push_back(PinFacade(pinID, aNodeGraphFacade));
			}
		}

		return pinFacades;
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedInputPinFacades() const
	{
		return GetPinFacadesFiltered([](const Pin& aPin)-> bool
			{
				return aPin.mConnectedPinIDs.empty() && Internal::GetPinTypeManager().GetPinType(aPin.mTypeID).mFlowType == eFlowType::Input;
			},
			*this
		);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedOutputPinFacades() const
	{
		return GetPinFacadesFiltered([](const Pin& aPin)-> bool
			{
				return aPin.mConnectedPinIDs.empty() && Internal::GetPinTypeManager().GetPinType(aPin.mTypeID).mFlowType == eFlowType::Output;
			},
			*this
		);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedPinFacadesByFlowType(const eFlowType aFlowType) const
	{
		switch (aFlowType)
		{
		case eFlowType::Input:
			return GetNonConnectedInputPinFacades();
			break;
		case eFlowType::Output:
			return GetNonConnectedOutputPinFacades();
			break;
		default:
			break;
		}
		return std::vector<PinFacade>();
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedPinFacadesByFlowTypeAndDataType(const eFlowType aFlowType, const GenericDataTypeFacade aDataTypeFacade) const
	{
		return GetPinFacadesFiltered(
			[aFlowType, dataTypeID = aDataTypeFacade.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Internal::GetPinTypeManager().GetPinType(aPin.mTypeID);
				return aPin.mConnectedPinIDs.empty() && pinType.mFlowType == aFlowType && pinType.mGenericDataTypeID == dataTypeID;
			},
			*this
		);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedPinFacadesByFlowTypeAndRelatedDataTypes(const eFlowType aFlowType, const GenericDataTypeFacade aDataTypeFacade) const
	{
		return GetPinFacadesFiltered(
			[aFlowType, dataTypeID = aDataTypeFacade.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Internal::GetPinTypeManager().GetPinType(aPin.mTypeID);
				const bool a = aPin.mConnectedPinIDs.empty() && pinType.mFlowType == aFlowType;

				return a && Internal::AreDataTypesLinkable(SelectByFlowType(aFlowType, dataTypeID, pinType.mGenericDataTypeID), pinType.mGenericDataTypeID);
			},
			*this
		);
	}


	std::vector<LinkFacade> NodeGraphFacade::GetLinkFacades(const bool aIncludeDestroyed) const
	{
		const std::vector<Link>& links = GetNodeGraph().mLinks;
		std::vector<LinkFacade> linkFacades;
		linkFacades.reserve(links.size());

		for (LinkID linkID{ 0 }; linkID < links.size(); linkID++)
		{
			const Link& link = links[linkID];
			if (!aIncludeDestroyed && link.mIsDestroyed)
			{
				continue;
			}

			linkFacades.emplace_back(linkID, *this);
		}

		return linkFacades;
	}

	NodeFacade NodeGraphFacade::CreateNode(const NodeTypeFacade& aNodeTypeFacade, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateNode(mNodeGraphVariant, aNodeTypeFacade.GetID(), aPosition, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}

	NodeFacade NodeGraphFacade::CreateNode(const std::string_view aName, bool& aSuccess, const Vec2 aPosition, CommandTracker* const aCommandTracker, const bool aCreateIfNameNotFound)
	{
		const NodeID nodeID = Internal::CreateNode(mNodeGraphVariant, aName, aSuccess, aPosition, aCreateIfNameNotFound, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}

	NodeFacade NodeGraphFacade::CreateNodeAutoLink(const NodeTypeFacade aNodeTypeFacade, const PinID aConnection, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateNodeAutoLink(mNodeGraphVariant, aNodeTypeFacade.GetID(), aConnection, aPosition, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}

	/*NodeFacade NodeGraphFacade::CreateGetterNode(const VariableFacade aVariableFacade, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateGetterNode(GetNodeGraph(), aVariableFacade.GetID(), aVariableFacade.GetClass(), aVariableFacade.GetDataType().GetID(), aPosition, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}

	NodeFacade NodeGraphFacade::CreateSetterNode(const VariableFacade aVariableFacade, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateSetterNode(GetNodeGraph(), aVariableFacade.GetID(), aVariableFacade.GetClass(), aVariableFacade.GetDataType().GetID(), aPosition, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}*/

	void NodeGraphFacade::DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, CommandTracker* const aCommandTracker)
	{
		Internal::DestroySelection(aNodeIDs, aLinkIDs, GetNodeGraph(), aCommandTracker);
	}

	LinkFacade NodeGraphFacade::TryCreateLink(const PinFacade aPinFacade1, const PinFacade aPinFacade2, CommandTracker* const aCommandTracker)
	{
		const LinkID linkID = Internal::TryCreateLink(GetNodeGraph(), aPinFacade1.GetID(), aPinFacade2.GetID(), aCommandTracker);
		return LinkFacade(linkID, *this);
	}

	void NodeGraphFacade::CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, CommandTracker* const aCommandTracker)
	{
		Internal::CommitNodeDrag(aNodeDragData, GetNodeGraph(), aCommandTracker);
	}

	void NodeGraphFacade::ReplaceTemplateNode(const NodeFacade aReplaceNodeFacade, const DataTypeFacade aDataTypeFacade, CommandTracker* const aCommandTracker)
	{
		Internal::ReplaceNode(GetNodeGraph(), aReplaceNodeFacade.GetID(), aDataTypeFacade.GetID(), aCommandTracker);
	}

	void NodeGraphFacade::ReplaceTemplateNode(const PinFacade aReplacePinFacade, const DataTypeFacade aDataTypeFacade, CommandTracker* const aCommandTracker)
	{
		Internal::ReplaceTemplateNode(GetNodeGraph(), aReplacePinFacade.GetNodeID(), aDataTypeFacade.GetID(), aCommandTracker);
	}

	const NodeGraphVariantHandle& NodeGraphFacade::GetVariant() const
	{
		return mNodeGraphVariant;
	}

	bool operator==(const NodeGraphFacade& a, const NodeGraphFacade& b)
	{
		return a.mNodeGraphVariant == b.mNodeGraphVariant;
	}

	NodeGraphFacade::operator bool() const
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
