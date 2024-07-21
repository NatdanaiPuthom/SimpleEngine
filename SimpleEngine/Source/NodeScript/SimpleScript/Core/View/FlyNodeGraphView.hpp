#pragma once
#include "../ScriptDefines.hpp"
#include "NodeView.hpp"
#include "PinView.hpp"

namespace FLY_NAMESPACE
{
	
	class NodeGraph;

	class NodeGraphView final
	{
	public:

		NodeGraphView() = default;
		NodeGraphView(NodeGraph& aNodeGraph);

		std::vector<NodeView> GetNodes(bool aIncludeDestroyed = false) const;
		std::vector<PinView> GetPins(bool aIncludeDestroyed = false) const;
		NodeGraph& GetNodeGraph();

	protected:

		NodeGraph* mNodeGraph = nullptr;

		//eNodeGraphType mNodeGraphType = eNodeGraphType::Event;

	};
}