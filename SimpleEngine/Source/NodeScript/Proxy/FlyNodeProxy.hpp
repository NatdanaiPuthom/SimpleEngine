#pragma once
#include "../FlyDefines.hpp"
#include "../Node/FlyNode.hpp"
#include "../Node/FlyNodeTrait.hpp"
#include "FlyPinProxy.hpp"
#include "FlyVariableProxy.hpp"
#include "FlyNodeTypeProxy.hpp"

namespace FLY_NAMESPACE
{

	class NodeType;
	class NodeGraphProxy;

	class NodeProxy final
	{
	public:

		NodeProxy() = default;
		NodeProxy(NodeID aNodeID, const NodeGraphProxy& aNodeGraphProxy);
		NodeProxy(NodeID aNodeID, const NodeGraphVariantHandle& aNodeGraphVariantHandle);

		[[nodiscard]] Vec2 GetPosition() const;
		[[nodiscard]] bool IsDestroyed() const;

		[[nodiscard]] std::vector<PinProxy> GetInputPins() const;
		[[nodiscard]] std::vector<PinProxy> GetOutputPins() const;
		[[nodiscard]] std::vector<PinProxy> GetSplitInputPins() const;
		[[nodiscard]] std::vector<PinProxy> GetSplitOutputPins() const;

		[[nodiscard]] NodeID GetID() const;

		[[nodiscard]] eNodeTrait GetTraits() const;
		[[nodiscard]] bool IsAccessor() const;
		[[nodiscard]] EventID GetEventID() const;

		[[nodiscard]] bool HasAnyConnectedLinks() const;
		[[nodiscard]] bool IsReplacable() const;

		//[[nodiscard]] VariableProxy GetVariableProxy() const;
		[[nodiscard]] NodeTypeProxy GetNodeType() const;

		void Destroy(CommandTracker* aCommandTracker);
		void DestroyConnectedLinks(CommandTracker* aCommandTracker);

		void SetPosition(Vec2 aPosition, CommandTracker* aCommandTracker = nullptr);

		[[nodiscard]] NodeGraph& GetNodeGraph() const;

		[[nodiscard]] friend bool operator==(const NodeProxy& a, const NodeProxy& b);

		[[nodiscard]] explicit operator bool() const;

	private:

		[[nodiscard]] const Node& GetNode() const;

		[[nodiscard]] std::vector<PinProxy> GetPins(const std::vector<PinID>& aPinIDs) const;
		[[nodiscard]] std::vector<PinProxy> GetPins(const eIODirection aIODirection) const;
		[[nodiscard]] std::vector<PinProxy> GetSplitPins(const eIODirection aIODirection) const;

	private:

		NodeGraphVariantHandle mNodeGraphVariant;
		NodeID mNodeID = InvalidID<NodeID>();

	};
}