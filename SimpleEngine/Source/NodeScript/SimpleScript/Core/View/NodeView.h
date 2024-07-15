#pragma once
#include "../ScriptDefines.h"
#include "../Node/Node.h"
#include "PinView.h"

namespace SCR
{
	struct NodeType;

	class NodeView final
	{
	public:

		NodeView(NodeID aNodeID, const NodeGraph& aNodeGraph);

		const std::string& GetNodeTypeName() const;
		ScriptVec2 GetPosition() const;
		bool IsDestroyed() const;

		std::vector<PinView> GetInputPins() const;
		std::vector<PinView> GetOutputPins() const;

	private:

		const Node& GetNode() const;
		const NodeType& GetNodeType() const;

		std::vector<PinView> GetPinViews(const eFlowType aFlowType) const;

	private:

		NodeID myNodeID = InvalidID<NodeID>();
		const NodeGraph* myNodeGraph = nullptr;

	};
}