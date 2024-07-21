#pragma once
#include "../ScriptDefines.hpp"
#include "../Node/Node.hpp"
#include "../Node/NodeTrait.hpp"
#include "PinView.hpp"

namespace FLY_NAMESPACE
{
	struct NodeType;

	class NodeView final
	{
	public:

		NodeView(NodeID aNodeID, const NodeGraph& aNodeGraph);

		const std::string& GetNodeTypeName() const;
		std::string GetShortName() const;
		Vec2 GetPosition() const;
		bool IsDestroyed() const;

		std::vector<PinView> GetInputPins() const;
		std::vector<PinView> GetOutputPins() const;

		NodeID GetID() const;

		eNodeTrait GetTraits() const;
		EventID GetEventID() const;

		const NodeGraph& GetNodeGraph() const;

		bool operator==(const NodeView& aOther) const;
		bool operator!=(const NodeView& aOther) const;

	private:

		const Node& GetNode() const;
		const NodeType& GetNodeType() const;

		std::vector<PinView> GetPinViews(const eFlowType aFlowType) const;

	private:

		NodeID mNodeID = InvalidID<NodeID>();
		const NodeGraph* mNodeGraph = nullptr;

	};
}