#include "FlyNodeGraphFacade.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "FlyFunctionFacade.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Fly.hpp"

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

	NodeGraphFacade::NodeGraphFacade(NodeGraphVariant&& aNodeGraphVariant)
		: mNodeGraphVariant(std::forward<NodeGraphVariant>(aNodeGraphVariant))
	{
	}

	NodeGraphFacade::NodeGraphFacade(const NodeGraphVariant& aNodeGraphVariant)
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
		return std::visit([&](auto& aArg) -> NodeGraph&
			{
				if constexpr (std::same_as<EventGraph*, std::decay_t<decltype(aArg)>>)
				{
					return aArg->mNodeGraph;
				}
				else
				{
					return Global::GetNodeTypeManager().GetFunction(std::get<FunctionIDWrapper>(mNodeGraphVariant).mID).mNodeGraph;
				}
			}, mNodeGraphVariant
		);
	}

	const NodeGraph& NodeGraphFacade::GetNodeGraph() const
	{
		return std::visit([&](const auto& aArg) -> const NodeGraph&
			{
				if constexpr (std::same_as<EventGraph*, std::decay_t<decltype(aArg)>>)
				{
					return aArg->mNodeGraph;
				}
				else
				{
					return Global::GetNodeTypeManager().GetFunction(std::get<FunctionIDWrapper>(mNodeGraphVariant).mID).mNodeGraph;
				}
			}, mNodeGraphVariant
		);
	}

	eNodeGraphType NodeGraphFacade::GetType() const
	{
		return std::visit([&](auto& aArg) -> eNodeGraphType
			{
				if constexpr (std::same_as<EventGraph*, std::decay_t<decltype(aArg)>>)
				{
					return eNodeGraphType::EventGraph;
				}
				else
				{
					return eNodeGraphType::Function;
				}
			}, mNodeGraphVariant
		);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedInputPinFacades() const
	{
		return FLY_NAMESPACE::GetNonConnectedInputPinFacades(*this);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedOutputPinFacades() const
	{
		return FLY_NAMESPACE::GetNonConnectedOutputPinFacades(*this);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedPinFacadesByFlowType(eFlowType aFlowType) const
	{
		return FLY_NAMESPACE::GetNonConnectedPinFacadesByFlowType(*this, aFlowType);
	}

	std::vector<PinFacade> NodeGraphFacade::GetNonConnectedPinFacadesByFlowTypeAndDataType(const eFlowType aFlowType, const DataTypeFacade aDataTypeFacade) const
	{
		return FLY_NAMESPACE::GetNonConnectedPinFacadesByFlowTypeAndDataType(*this, aFlowType, aDataTypeFacade);
	}

	NodeFacade NodeGraphFacade::CreateNode(const NodeTypeFacade& aNodeTypeFacade, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		return FLY_NAMESPACE::CreateNode(*this, aNodeTypeFacade, aPosition, aCommandTracker);
	}

	NodeFacade NodeGraphFacade::CreateNode(std::string_view aName, bool& aSuccess, Vec2 aPosition, CommandTracker* aCommandTracker, bool aCreateIfNameNotFound)
	{
		return FLY_NAMESPACE::CreateNode(*this, aName, aSuccess, aPosition, aCommandTracker, aCreateIfNameNotFound);
	}

	NodeFacade NodeGraphFacade::CreateNodeAutoLink(NodeTypeFacade aNodeTypeFacade, PinID aConnection, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		return FLY_NAMESPACE::CreateNodeAutoLink(*this, aNodeTypeFacade, aConnection, aPosition, aCommandTracker);
	}

	NodeFacade NodeGraphFacade::CreateGetterNode(VariableFacade aVariableFacade, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		return FLY_NAMESPACE::CreateGetterNode(*this, aVariableFacade, aPosition, aCommandTracker);
	}

	NodeFacade NodeGraphFacade::CreateSetterNode(VariableFacade aVariableFacade, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		return FLY_NAMESPACE::CreateSetterNode(*this, aVariableFacade, aPosition, aCommandTracker);
	}

	void NodeGraphFacade::DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, CommandTracker* const aCommandTracker)
	{
		FLY_NAMESPACE::DestroySelection(aNodeIDs, aLinkIDs, *this, aCommandTracker);
	}

	LinkFacade NodeGraphFacade::TryCreateLink(PinFacade aPinFacade1, PinFacade aPinFacade2, CommandTracker* const aCommandTracker)
	{
		return FLY_NAMESPACE::TryCreateLink(aPinFacade1, aPinFacade2, *this, aCommandTracker);
	}

	void NodeGraphFacade::CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, CommandTracker* const aCommandTracker)
	{
		FLY_NAMESPACE::CommitNodeDrag(aNodeDragData, *this, aCommandTracker);
	}

	void NodeGraphFacade::ReplaceTemplateNode(NodeFacade aReplaceNodeFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker)
	{
		FLY_NAMESPACE::ReplaceTemplateNode(aReplaceNodeFacade, *this, aDataTypeFacade, aCommandTracker);
	}

	void NodeGraphFacade::ReplaceTemplateNode(PinFacade aReplacePinFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker)
	{
		FLY_NAMESPACE::ReplaceTemplateNode(aReplacePinFacade, *this, aDataTypeFacade, aCommandTracker);
	}

	const NodeGraphVariant& NodeGraphFacade::GetVariant() const
	{
		return mNodeGraphVariant;
	}

	bool operator==(const NodeGraphFacade& a, const NodeGraphFacade& b)
	{
		if (a.GetType() != b.GetType())
		{
			return false;
		}

		switch (a.mNodeGraphVariant.index())
		{
		case 0:
			return std::get<EventGraph*>(a.mNodeGraphVariant) == std::get<EventGraph*>(b.mNodeGraphVariant);
			break;
		case 1:
			return std::get<FunctionIDWrapper>(a.mNodeGraphVariant) == std::get<FunctionIDWrapper>(b.mNodeGraphVariant);
		default:
			break;
		}
		return false;
		/*return std::visit([](const auto& aArg, const auto& bArg) -> bool
			{
				using T = std::decay_t<decltype(aArg)>;
				if constexpr (std::is_same_v<T, EventGraph*>) {
					return aArg == bArg;
				}
				else if constexpr (std::is_same_v<T, NodeGraphFacade::FunctionIDWrapper>) {
					return aArg.mID == bArg.mID;
				}
				else {
					return false;
				}
			}, a.mNodeGraph, b.mNodeGraph
		);*/
	}

	NodeGraphFacade::operator bool() const
	{
		return std::visit([](const auto& aArg) -> bool
			{
				using T = std::decay_t<decltype(aArg)>;
				if constexpr (std::is_same_v<T, EventGraph*>)
				{
					return aArg != nullptr;
				}
				else if constexpr (std::is_same_v<T, FunctionIDWrapper>)
				{
					return aArg.mID != InvalidID<FunctionID>();
				}
				else 
				{
					return false;
				}
			}, mNodeGraphVariant
		);
	}


}
