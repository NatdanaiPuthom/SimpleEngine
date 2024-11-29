#pragma once
#include "../FlyDefines.hpp"
#include "../Node/FlyNode.hpp"
#include "../Node/FlyNodeTrait.hpp"
#include "FlyPinFacade.hpp"
#include "FlyVariableFacade.hpp"
#include "FlyNodeTypeFacade.hpp"

namespace FLY_NAMESPACE
{
	struct NodeType;
	class NodeGraphFacade;

	class NodeFacade final
	{
	public:

		NodeFacade() = default;
		NodeFacade(NodeID aNodeID, const NodeGraphFacade& aNodeGraphFacade);

		[[nodiscard]] Vec2 GetPosition() const;
		[[nodiscard]] bool IsDestroyed() const;

		[[nodiscard]] std::vector<PinFacade> GetInputPinFacades() const;
		[[nodiscard]] std::vector<PinFacade> GetOutputPinFacades() const;

		[[nodiscard]] NodeID GetID() const;

		[[nodiscard]] eNodeTrait GetTraits() const;
		[[nodiscard]] bool IsAccessor() const;
		[[nodiscard]] EventID GetEventID() const;

		[[nodiscard]] bool HasAnyConnectedLinks() const;
		[[nodiscard]] bool IsReplacable() const;

		[[nodiscard]] VariableFacade GetVariableFacade() const;
		[[nodiscard]] NodeTypeFacade GetNodeTypeFacade() const;

		void Destroy(CommandTracker* aCommandTracker);
		void DestroyConnectedLinks(CommandTracker* aCommandTracker);

		void SetPosition(Vec2 aPosition, CommandTracker* aCommandTracker = nullptr);

		[[nodiscard]] NodeGraph& GetNodeGraph() const;

		friend bool operator==(const NodeFacade& a, const NodeFacade& b);

		explicit operator bool() const;

	private:

		[[nodiscard]] const Node& GetNode() const;
		[[nodiscard]] const NodeType& GetNodeType() const;

		[[nodiscard]] std::vector<PinFacade> GetPinFacades(const eFlowType aFlowType) const;

	private:

		NodeGraphVariant mNodeGraphVariant;
		NodeID mNodeID = InvalidID<NodeID>();

	};
}