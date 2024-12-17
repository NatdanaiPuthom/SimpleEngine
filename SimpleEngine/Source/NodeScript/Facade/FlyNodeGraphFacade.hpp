#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeFacade.hpp"
#include "FlyPinFacade.hpp"
#include "FlyLinkFacade.hpp"
#include "FlyDataTypeFacade.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include "../Node/NodeDragData.hpp"

namespace FLY_NAMESPACE
{

	
	
	class NodeGraph;
	class EventGraph;
	class FunctionFacade;
	class ClassFacade;
	class NodeTypeFacade;
	class VariableFacade;


	enum class eNodeGraphType
	{
		None,
		EventGraph,
		Function
	};

	class NodeGraphFacade final
	{
	public:

		NodeGraphFacade() = default;
		explicit NodeGraphFacade(const FunctionFacade& aFunctionFacade);
		explicit NodeGraphFacade(EventGraph& aEventGraph);
		explicit NodeGraphFacade(NodeGraphVariantHandle&& aNodeGraphVariant);
		explicit NodeGraphFacade(const NodeGraphVariantHandle& aNodeGraphVariant);

		[[nodiscard]] std::vector<NodeFacade> GetNodeFacades(bool aIncludeDestroyed = false) const;
		[[nodiscard]] std::vector<PinFacade> GetPinFacades(bool aIncludeDestroyed = false) const;

		[[nodiscard]] NodeGraph& GetNodeGraph();
		[[nodiscard]] const NodeGraph& GetNodeGraph() const;

		[[nodiscard]] eNodeGraphType GetType() const;

		[[nodiscard]] std::vector<PinFacade> GetNonConnectedInputPinFacades() const;
		[[nodiscard]] std::vector<PinFacade> GetNonConnectedOutputPinFacades() const;
		[[nodiscard]] std::vector<PinFacade> GetNonConnectedPinFacadesByFlowType(eFlowType aFlowType) const;
		[[nodiscard]] std::vector<PinFacade> GetNonConnectedPinFacadesByFlowTypeAndDataType(eFlowType aFlowType, DataTypeFacade aDataTypeFacade) const;
		[[nodiscard]] std::vector<PinFacade> GetNonConnectedPinFacadesByFlowTypeAndRelatedDataTypes(eFlowType aFlowType, DataTypeFacade aDataTypeFacade) const;

		[[nodiscard]] std::vector<LinkFacade> GetLinkFacades(bool aIncludeDestroyed = false);

		NodeFacade CreateNode(const NodeTypeFacade& aNodeTypeFacade, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeFacade CreateNode(std::string_view aName, bool& aSuccess, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
		NodeFacade CreateNodeAutoLink(NodeTypeFacade aNodeTypeFacade, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeFacade CreateGetterNode(VariableFacade aVariableFacade, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeFacade CreateSetterNode(VariableFacade aVariableFacade, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, CommandTracker* aCommandTracker);
		LinkFacade TryCreateLink(PinFacade aPinFacade1, PinFacade aPinFacade2, CommandTracker* aCommandTracker);

		void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aNodeDragData, CommandTracker* aCommandTracker);

		void ReplaceTemplateNode(NodeFacade aReplaceNodeFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker);
		void ReplaceTemplateNode(PinFacade aReplacePinFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker);

		[[nodiscard]] const NodeGraphVariantHandle& GetVariant() const;

		friend bool operator==(const NodeGraphFacade& a, const NodeGraphFacade& b);

		explicit operator bool() const;

	private:

		NodeGraphVariantHandle mNodeGraphVariant;

	};
}

template<>
struct std::hash<FLY_NAMESPACE::NodeGraphFacade>
{
	size_t operator()(const FLY_NAMESPACE::NodeGraphFacade& aNodeGraphFacade) const
	{
		return reinterpret_cast<size_t>(&aNodeGraphFacade.GetNodeGraph());
	}
};