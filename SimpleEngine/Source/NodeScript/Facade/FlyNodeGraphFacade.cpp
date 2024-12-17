#include "FlyNodeGraphFacade.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "FlyFunctionFacade.hpp"
#include "../Global/FlyGlobal.hpp"
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

		for (NodeID nodeID = 0; nodeID < nodes.size(); ++nodeID)
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

		for (PinID pinID = 0; pinID < pins.size(); ++pinID)
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

		for (PinID pinID = 0; pinID < nodeGraph.mPins.size(); ++pinID)
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
				return aPin.mConnectedPinIDs.empty() && Global::GetPinTypeManager().GetPinType(aPin.mTypeID).mFlowType == eFlowType::Input;
			},
			*this
		);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedOutputPinFacades() const
	{
		return GetPinFacadesFiltered([](const Pin& aPin)-> bool
			{
				return aPin.mConnectedPinIDs.empty() && Global::GetPinTypeManager().GetPinType(aPin.mTypeID).mFlowType == eFlowType::Output;
			},
			*this
		);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedPinFacadesByFlowType(eFlowType aFlowType) const
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

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedPinFacadesByFlowTypeAndDataType(const eFlowType aFlowType, const DataTypeFacade aDataTypeFacade) const
	{
		return GetPinFacadesFiltered(
			[aFlowType, dataTypeID = aDataTypeFacade.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(aPin.mTypeID);
				return aPin.mConnectedPinIDs.empty() && pinType.mFlowType == aFlowType && pinType.mDataTypeID == dataTypeID;
			},
			*this
		);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedPinFacadesByFlowTypeAndRelatedDataTypes(const eFlowType aFlowType, const DataTypeFacade aDataTypeFacade) const
	{
		return GetPinFacadesFiltered(
			[aFlowType, dataTypeID = aDataTypeFacade.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(aPin.mTypeID);
				return aPin.mConnectedPinIDs.empty() && pinType.mFlowType == aFlowType && Global::GetDataTypeManager().AreDataTypesRelated(dataTypeID, pinType.mDataTypeID);
			},
			*this
		);
	}


	std::vector<LinkFacade> NodeGraphFacade::GetLinkFacades(const bool aIncludeDestroyed)
	{
		const std::vector<Link>& links = GetNodeGraph().mLinks;
		std::vector<LinkFacade> linkFacades;
		linkFacades.reserve(links.size());

		for (LinkID linkID = 0; linkID < links.size(); linkID++)
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
		const NodeID nodeID = Internal::CreateNode(GetVariant(), aNodeTypeFacade.GetID(), aPosition, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}

	NodeFacade NodeGraphFacade::CreateNode(std::string_view aName, bool& aSuccess, Vec2 aPosition, CommandTracker* aCommandTracker, bool aCreateIfNameNotFound)
	{
		const NodeID nodeID = Internal::CreateNode(GetVariant(), aName, aSuccess, aPosition, aCreateIfNameNotFound, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}

	NodeFacade NodeGraphFacade::CreateNodeAutoLink(NodeTypeFacade aNodeTypeFacade, PinID aConnection, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateNodeAutoLink(GetVariant(), aNodeTypeFacade.GetID(), aConnection, aPosition, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}

	NodeFacade NodeGraphFacade::CreateGetterNode(VariableFacade aVariableFacade, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateGetterNode(GetNodeGraph(), aVariableFacade.GetID(), aVariableFacade.GetClass(), aVariableFacade.GetDataType().GetID(), aPosition, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}

	NodeFacade NodeGraphFacade::CreateSetterNode(VariableFacade aVariableFacade, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateSetterNode(GetNodeGraph(), aVariableFacade.GetID(), aVariableFacade.GetClass(), aVariableFacade.GetDataType().GetID(), aPosition, aCommandTracker);
		return NodeFacade(nodeID, *this);
	}

	void NodeGraphFacade::DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, CommandTracker* const aCommandTracker)
	{
		Internal::DestroySelection(aNodeIDs, aLinkIDs, GetNodeGraph(), aCommandTracker);
	}

	LinkFacade NodeGraphFacade::TryCreateLink(PinFacade aPinFacade1, PinFacade aPinFacade2, CommandTracker* const aCommandTracker)
	{
		const LinkID linkID = Internal::TryCreateLink(GetNodeGraph(), aPinFacade1.GetID(), aPinFacade2.GetID(), aCommandTracker);
		return LinkFacade(linkID, *this);
	}

	void NodeGraphFacade::CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, CommandTracker* const aCommandTracker)
	{
		Internal::CommitNodeDrag(aNodeDragData, GetNodeGraph(), aCommandTracker);
	}

	void NodeGraphFacade::ReplaceTemplateNode(NodeFacade aReplaceNodeFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker)
	{
		Internal::ReplaceTemplateNode(GetNodeGraph(), aReplaceNodeFacade.GetID(), aDataTypeFacade.GetID(), aCommandTracker);
	}

	void NodeGraphFacade::ReplaceTemplateNode(PinFacade aReplacePinFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker)
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
		/*return std::visit([](const auto& aArg, const auto& bArg) -> bool
			{
				using T1 = std::decay_t<decltype(aArg)>;
				using T2 = std::decay_t<decltype(bArg)>;
				if constexpr (std::same_as<T1, T2>)
				{
					return aArg == bArg;
				}
				else
				{
					return false;
				}
			}, a.mNodeGraphVariant, b.mNodeGraphVariant
		);*/
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
