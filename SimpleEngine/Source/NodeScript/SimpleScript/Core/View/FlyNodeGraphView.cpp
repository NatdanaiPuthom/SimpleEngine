#include "FlyNodeGraphView.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "FlyFunctionView.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Fly.hpp"

namespace FLY_NAMESPACE
{

	NodeGraphView::NodeGraphView(const FunctionView& aFunctionView)
		: mNodeGraphVariant(FunctionIDWrapper(aFunctionView.GetID()))
	{
	}

	NodeGraphView::NodeGraphView(EventGraph& aEventGraph)
		: mNodeGraphVariant(&aEventGraph)
	{
	}

	NodeGraphView::NodeGraphView(NodeGraphVariant&& aNodeGraphVariant)
		: mNodeGraphVariant(std::forward<NodeGraphVariant>(aNodeGraphVariant))
	{
	}

	NodeGraphView::NodeGraphView(const NodeGraphVariant& aNodeGraphVariant)
		: mNodeGraphVariant(aNodeGraphVariant)
	{
	}

	std::vector<NodeView> NodeGraphView::GetNodes(const bool aIncludeDestroyed) const
	{
		const std::vector<Node>& nodes = GetNodeGraph().mNodes;

		std::vector<NodeView> nodeViews;
		nodeViews.reserve(nodes.size());

		for (NodeID nodeID = 0; nodeID < nodes.size(); ++nodeID)
		{
			NodeView nodeView(nodeID, *this);
			if (!aIncludeDestroyed && nodeView.IsDestroyed())
			{
				continue;
			}
			nodeViews.push_back(nodeView);
		}

		return nodeViews;
	}

	std::vector<PinView> NodeGraphView::GetPins(const bool aIncludeDestroyed) const
	{
		const std::vector<Pin>& pins = GetNodeGraph().mPins;

		std::vector<PinView> pinViews;
		pinViews.reserve(pins.size());

		for (PinID pinID = 0; pinID < pins.size(); ++pinID)
		{
			PinView pinView(pinID, *this);
			NodeView nodeView(pinView.GetNodeID(), *this);
			if (!aIncludeDestroyed && nodeView.IsDestroyed())
			{
				continue;
			}
			pinViews.push_back(pinView);
		}

		return pinViews;
	}

	NodeGraph& NodeGraphView::GetNodeGraph()
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

	const NodeGraph& NodeGraphView::GetNodeGraph() const
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

	eNodeGraphType NodeGraphView::GetType() const
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

	NodeView NodeGraphView::CreateNode(const NodeTypeView& aNodeTypeView, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		return Fly::CreateNode(*this, aNodeTypeView, aPosition, aCommandTracker);
	}

	NodeView NodeGraphView::CreateNode(std::string_view aName, bool& aSuccess, Vec2 aPosition, CommandTracker* aCommandTracker, bool aCreateIfNameNotFound)
	{
		return Fly::CreateNode(*this, aName, aSuccess, aPosition, aCommandTracker, aCreateIfNameNotFound);
	}

	NodeView NodeGraphView::CreateNodeAutoLink(NodeTypeView aNodeTypeView, PinID aConnection, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		return Fly::CreateNodeAutoLink(*this, aNodeTypeView, aConnection, aPosition, aCommandTracker);
	}

	NodeView NodeGraphView::CreateGetterNode(const ClassView& aClassView, VariableView aVariableView, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		return Fly::CreateGetterNode(aClassView, *this, aVariableView, aPosition, aCommandTracker);
	}

	NodeView NodeGraphView::CreateSetterNode(const ClassView& aClassView, VariableView aVariableView, Vec2 aPosition, CommandTracker* aCommandTracker)
	{
		return Fly::CreateSetterNode(aClassView, *this, aVariableView, aPosition, aCommandTracker);
	}

	void NodeGraphView::DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, CommandTracker* const aCommandTracker)
	{
		Fly::DestroySelection(aNodeIDs, aLinkIDs, *this, aCommandTracker);
	}

	LinkView NodeGraphView::TryCreateLink(PinView aPinView1, PinView aPinView2, CommandTracker* const aCommandTracker)
	{
		return Fly::TryCreateLink(aPinView1, aPinView2, *this, aCommandTracker);
	}

	const NodeGraphVariant& NodeGraphView::GetVariant() const
	{
		return mNodeGraphVariant;
	}

	std::variant<NodeGraph*, EventGraph*> NodeGraphView::GetAs()
	{
		return std::visit([&](auto& aArg) -> std::variant<NodeGraph*, EventGraph*>
			{
				if constexpr (std::same_as<EventGraph*, std::decay_t<decltype(aArg)>>)
				{
					return aArg;
				}
				else
				{
					return &GetNodeGraph();
				}
			}, mNodeGraphVariant
		);
	}

	bool operator==(const NodeGraphView& a, const NodeGraphView& b)
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
				else if constexpr (std::is_same_v<T, NodeGraphView::FunctionIDWrapper>) {
					return aArg.mID == bArg.mID;
				}
				else {
					return false;
				}
			}, a.mNodeGraph, b.mNodeGraph
		);*/
	}

	NodeGraphView::operator bool() const
	{
		return std::get<EventGraph*>(mNodeGraphVariant) != nullptr;
	}


}
