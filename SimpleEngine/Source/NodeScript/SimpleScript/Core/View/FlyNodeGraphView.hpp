#pragma once
#include "../ScriptDefines.hpp"
#include "NodeView.hpp"
#include "PinView.hpp"
#include <variant>

namespace FLY_NAMESPACE
{
	
	class NodeGraph;
	class EventGraph;
	class FunctionView;

	enum class eNodeGraphType
	{
		EventGraph,
		Function
	};

	class NodeGraphView final
	{
		struct FunctionIDWrapper
		{
			FunctionID mID = InvalidID<FunctionID>();
		};
	public:

		NodeGraphView() = default;
		explicit NodeGraphView(const FunctionView& aFunctionView);
		explicit NodeGraphView(EventGraph& aEventGraph);

		std::vector<NodeView> GetNodes(bool aIncludeDestroyed = false) const;
		std::vector<PinView> GetPins(bool aIncludeDestroyed = false) const;

		NodeGraph& GetNodeGraph();
		const NodeGraph& GetNodeGraph() const;

		eNodeGraphType GetType() const;

		std::variant<NodeGraph*, EventGraph*> GetAs();

		bool operator==(const NodeGraphView& aOther) const;

	private:

		std::variant<EventGraph*, FunctionIDWrapper> mNodeGraph;

	};
}

template<>
struct std::hash<FLY_NAMESPACE::NodeGraphView>
{
	size_t operator()(const FLY_NAMESPACE::NodeGraphView& aNodeGraphView) const
	{
		return reinterpret_cast<size_t>(&aNodeGraphView.GetNodeGraph());
	}
};