#include "FlyNodeGraphView.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "FlyFunctionView.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Utilities/FlyMeta.hpp"

namespace FLY_NAMESPACE
{

	NodeGraphView::NodeGraphView(const FunctionView& aFunctionView)
		: mNodeGraph(FunctionIDWrapper(aFunctionView.GetID()))
	{
	}

	NodeGraphView::NodeGraphView(EventGraph& aEventGraph)
		: mNodeGraph(&aEventGraph)
	{
	}

	std::vector<NodeView> NodeGraphView::GetNodes(const bool aIncludeDestroyed) const
	{
		const std::vector<Node>& nodes = GetNodeGraph().mNodes;

		std::vector<NodeView> nodeViews;
		nodeViews.reserve(nodes.size());

		for (NodeID nodeID = 0; nodeID < nodes.size(); ++nodeID)
		{
			NodeView nodeView(nodeID, GetNodeGraph());
			if (!aIncludeDestroyed && nodeView.IsDestroyed())
			{
				continue;
			}
			nodeViews.push_back(nodeView);
		}

		return nodeViews;
	}

	std::vector<PinView> NodeGraphView::GetPins(bool aIncludeDestroyed) const
	{
		const std::vector<Pin>& pins = GetNodeGraph().mPins;

		std::vector<PinView> pinViews;
		pinViews.reserve(pins.size());

		for (PinID pinID = 0; pinID < pins.size(); ++pinID)
		{
			PinView pinView(pinID, GetNodeGraph());
			NodeView nodeView(pinView.GetNodeID(), GetNodeGraph());
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
					return Global::GetNodeTypeManager().GetFunction(std::get<FunctionIDWrapper>(mNodeGraph).mID).mNodeGraph;
				}
			}, mNodeGraph
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
					return Global::GetNodeTypeManager().GetFunction(std::get<FunctionIDWrapper>(mNodeGraph).mID).mNodeGraph;
				}
			}, mNodeGraph
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
			}, mNodeGraph
		);
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
			}, mNodeGraph
		);
	}

	bool NodeGraphView::operator==(const NodeGraphView& aOther) const
	{
		if (GetType() != aOther.GetType())
		{
			return false;
		}
		return std::visit([&](auto& aArg) -> bool
			{
				if constexpr (std::same_as<EventGraph*, std::decay_t<decltype(aArg)>>)
				{
					return std::get<EventGraph*>(mNodeGraph) == std::get<EventGraph*>(aOther.mNodeGraph);
				}
				else
				{
					return std::get<FunctionIDWrapper>(mNodeGraph).mID == std::get<FunctionIDWrapper>(aOther.mNodeGraph).mID;
				}
			}, mNodeGraph
		);
	}
}
