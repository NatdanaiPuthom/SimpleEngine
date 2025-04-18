#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeProxy.hpp"
#include "FlyPinProxy.hpp"
#include "FlyLinkProxy.hpp"
#include "FlyDataTypeProxy.hpp"
#include "FlyGenericDataTypeProxy.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include "../Node/FlyNodeDragData.hpp"
#include "ProxyIterators/FlyProxyContextIterator.hpp"
#include "ProxyIterators/FlyNodeProxyIterator.hpp"
#include <span>

namespace FLY_NAMESPACE
{

	using PinProxyIterator = ProxyContextIterator<PinID, PinProxy>;
	using PinProxyIteratorService = ProxyContextIteratorService<PinID, PinProxyIterator, PinProxy>;
	using LinkProxyIterator = ProxyContextIterator<LinkID, LinkProxy>;
	using LinkProxyIteratorService = ProxyContextIteratorService<LinkID, LinkProxyIterator>;
	
	class NodeGraph;
	class EventGraph;
	class FunctionProxy;
	class ClassProxy;
	class NodeTypeProxy;
	class VariableProxy;


	enum class eNodeGraphType
	{
		None,
		EventGraph,
		Function
	};

	class NodeGraphProxy final
	{
	public:

		NodeGraphProxy() = default;
		explicit NodeGraphProxy(const FunctionProxy& aFunctionProxy);
		explicit NodeGraphProxy(EventGraph& aEventGraph);
		explicit NodeGraphProxy(NodeGraphVariantHandle&& aNodeGraphVariant);
		explicit NodeGraphProxy(const NodeGraphVariantHandle& aNodeGraphVariant);
		
		[[nodiscard]] NodeProxyIteratorService IterateNodes(Predicate<NodeProxy> aFilterPredicate) const;
		[[nodiscard]] NodeProxyIteratorService IterateNodes(bool aIncludeDestroyed = false) const;
		[[nodiscard]] LinkProxyIteratorService IterateLinks(bool aIncludeDestroyed = false) const;

		[[nodiscard]] NodeGraph& GetNodeGraph();
		[[nodiscard]] const NodeGraph& GetNodeGraph() const;

		[[nodiscard]] eNodeGraphType GetType() const;

		[[nodiscard]] std::vector<PinProxy> GetNonConnectedInputPins() const;
		[[nodiscard]] std::vector<PinProxy> GetNonConnectedOutputPins() const;
		[[nodiscard]] std::vector<PinProxy> GetNonConnectedPinsByIODirection(eIODirection aIODirection) const;
		[[nodiscard]] std::vector<PinProxy> GetNonConnectedPinsByIODirectionAndDataType(eIODirection aIODirection, GenericDataTypeProxy aDataTypeProxy) const;
		[[nodiscard]] std::vector<PinProxy> GetNonConnectedPinsByIODirectionAndRelatedDataTypes(eIODirection aIODirection, GenericDataTypeProxy aDataTypeProxy) const;

		NodeProxy CreateNode(const NodeTypeProxy& aNodeTypeProxy, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeProxy CreateNode(std::string_view aName, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
		NodeProxy CreateNodeAutoLink(NodeTypeProxy aNodeTypeProxy, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		//NodeProxy CreateGetterNode(VariableProxy aVariableProxy, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		//NodeProxy CreateSetterNode(VariableProxy aVariableProxy, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

		void DestroySelection(std::span<NodeID> aNodeIDs, std::span<LinkID> aLinkIDs, CommandTracker* aCommandTracker);
		LinkProxy TryCreateLink(PinProxy aPin1, PinProxy aPin2, CommandTracker* aCommandTracker);

		void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, CommandTracker* aCommandTracker);

		void ReplaceTemplateNode(NodeProxy aReplaceNode, DataTypeProxy aDataType, CommandTracker* aCommandTracker);
		void ReplaceTemplateNode(PinProxy aReplacePin, DataTypeProxy aDataType, CommandTracker* aCommandTracker);

		[[nodiscard]] const NodeGraphVariantHandle& GetVariant() const;

		friend bool operator==(const NodeGraphProxy& a, const NodeGraphProxy& b);

		explicit operator bool() const;

	private:

		NodeGraphVariantHandle mNodeGraphVariant;

	};
}

template<>
struct std::hash<FLY_NAMESPACE::NodeGraphProxy>
{
	size_t operator()(const FLY_NAMESPACE::NodeGraphProxy& aNodeGraphProxy) const
	{
		return reinterpret_cast<size_t>(&aNodeGraphProxy.GetNodeGraph());
	}
};