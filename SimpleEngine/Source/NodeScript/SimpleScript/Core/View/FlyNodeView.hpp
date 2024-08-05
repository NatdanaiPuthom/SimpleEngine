#pragma once
#include "../FlyDefines.hpp"
#include "../Node/FlyNode.hpp"
#include "../Node/FlyNodeTrait.hpp"
#include "FlyPinView.hpp"

namespace FLY_NAMESPACE
{
	struct NodeType;
	class NodeGraphView;

	class NodeView final
	{
	public:

		NodeView() = default;
		NodeView(NodeID aNodeID, const NodeGraphView& aNodeGraphView);

		const std::string& GetNodeTypeName() const;
		std::string GetShortName() const;
		Vec2 GetPosition() const;
		bool IsDestroyed() const;

		std::vector<PinView> GetInputPinViews() const;
		std::vector<PinView> GetOutputPinViews() const;

		NodeID GetID() const;

		eNodeTrait GetTraits() const;
		EventID GetEventID() const;

		void Destroy(CommandTracker* aCommandTracker);
		void DestroyConnectedLinks(CommandTracker* aCommandTracker);

		const NodeGraph& GetNodeGraph() const;

		friend bool operator==(const NodeView& a, const NodeView& b);

		explicit operator bool() const;

	private:

		const Node& GetNode() const;
		const NodeType& GetNodeType() const;

		std::vector<PinView> GetPinViews(const eFlowType aFlowType) const;

	private:

		NodeGraphVariant mNodeGraphVariant;
		NodeID mNodeID = InvalidID<NodeID>();

	};
}