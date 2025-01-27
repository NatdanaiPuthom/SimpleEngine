#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeProxy.hpp"
#include "FlyPinProxy.hpp"
#include "FlyLinkProxy.hpp"
#include "FlyDataTypeProxy.hpp"
#include "FlyGenericDataTypeProxy.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include "../Node/FlyNodeDragData.hpp"

namespace FLY_NAMESPACE
{

	
	
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

		[[nodiscard]] std::vector<NodeProxy> GetNodeProxys(bool aIncludeDestroyed = false) const;
		[[nodiscard]] std::vector<PinProxy> GetPinProxys(bool aIncludeDestroyed = false) const;

		[[nodiscard]] NodeGraph& GetNodeGraph();
		[[nodiscard]] const NodeGraph& GetNodeGraph() const;

		[[nodiscard]] eNodeGraphType GetType() const;

		[[nodiscard]] std::vector<PinProxy> GetNonConnectedInputPins() const;
		[[nodiscard]] std::vector<PinProxy> GetNonConnectedOutputPins() const;
		[[nodiscard]] std::vector<PinProxy> GetNonConnectedPinsByFlowType(eFlowType aFlowType) const;
		[[nodiscard]] std::vector<PinProxy> GetNonConnectedPinsByFlowTypeAndDataType(eFlowType aFlowType, GenericDataTypeProxy aDataTypeProxy) const;
		[[nodiscard]] std::vector<PinProxy> GetNonConnectedPinsByFlowTypeAndRelatedDataTypes(eFlowType aFlowType, GenericDataTypeProxy aDataTypeProxy) const;

		[[nodiscard]] std::vector<LinkProxy> GetLinks(bool aIncludeDestroyed = false) const;

		NodeProxy CreateNode(const NodeTypeProxy& aNodeTypeProxy, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeProxy CreateNode(std::string_view aName, bool& aSuccess, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
		NodeProxy CreateNodeAutoLink(NodeTypeProxy aNodeTypeProxy, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		//NodeProxy CreateGetterNode(VariableProxy aVariableProxy, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		//NodeProxy CreateSetterNode(VariableProxy aVariableProxy, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, CommandTracker* aCommandTracker);
		LinkProxy TryCreateLink(PinProxy aPinProxy1, PinProxy aPinProxy2, CommandTracker* aCommandTracker);

		void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, CommandTracker* aCommandTracker);

		void ReplaceTemplateNode(NodeProxy aReplaceNodeProxy, DataTypeProxy aDataTypeProxy, CommandTracker* aCommandTracker);
		void ReplaceTemplateNode(PinProxy aReplacePinProxy, DataTypeProxy aDataTypeProxy, CommandTracker* aCommandTracker);

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