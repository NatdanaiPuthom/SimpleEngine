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

		[[nodiscard]] const std::string& GetNodeTypeName() const;
		[[nodiscard]] std::string GetShortName() const;
		[[nodiscard]] Vec2 GetPosition() const;
		[[nodiscard]] bool IsDestroyed() const;

		[[nodiscard]] std::vector<PinView> GetInputPinViews() const;
		[[nodiscard]] std::vector<PinView> GetOutputPinViews() const;

		[[nodiscard]] NodeID GetID() const;

		[[nodiscard]] eNodeTrait GetTraits() const;
		[[nodiscard]] EventID GetEventID() const;

		[[nodiscard]] bool HasAnyConnectedLinks() const;
		[[nodiscard]] bool IsReplacable() const;

		void Destroy(CommandTracker* aCommandTracker);
		void DestroyConnectedLinks(CommandTracker* aCommandTracker);

		void SetPosition(Vec2 aPosition, CommandTracker* aCommandTracker = nullptr);

		[[nodiscard]] const NodeGraph& GetNodeGraph() const;

		friend bool operator==(const NodeView& a, const NodeView& b);

		explicit operator bool() const;

	private:

		[[nodiscard]] const Node& GetNode() const;
		[[nodiscard]] const NodeType& GetNodeType() const;

		[[nodiscard]] std::vector<PinView> GetPinViews(const eFlowType aFlowType) const;

	private:

		NodeGraphVariant mNodeGraphVariant;
		NodeID mNodeID = InvalidID<NodeID>();

	};
}