#pragma once
#include "FlyDefines.hpp"
#include "Pin/FlyPin.hpp"
#include "Node/FlyNodeTrait.hpp"
#include "SystemTypes/FlyVec2.hpp"
#include "Node/FlyNodeRef.hpp"
#include "Variable/FlyVariableRef.hpp"
#include <string>
#include <variant>

namespace FLY_NAMESPACE
{

	class Class;
	class CommandTracker;
	class EventGraph;


	struct NodeGraphContext
	{
		NodeGraph& mNodeGraph;
		Class& script;
	};

	namespace Internal
	{

		using NodeGraphVariant = std::variant<NodeGraph*, EventGraph*>;


		CustomEventID CreateCustomEvent(std::string_view aName);
		FunctionID CreateFunction(std::string_view aName);
		NodeID CreateNode(NodeGraphVariant&& aNodeGraphVariant, NodeTypeID aNodeTypeID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateNode(NodeGraphVariant&& aNodeGraphVariant, std::string_view aName, bool& aSuccess, Vec2 aPosition, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker);
		NodeID CreateGetterNode(NodeGraph& aNodeGraph, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		NodeID CreateSetterNode(NodeGraph& aNodeGraph, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		NodeID CreateOperatorNode(NodeGraph& aNodeGraph, eNodeOperatorTrait aOperatorTrait, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, NodeID aNodeID, CommandTracker* aCommandTracker);

		void DestroyNode(NodeGraph& aNodeGraph, NodeID aNodeID, CommandTracker* aCommandTracker);
		void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker);
		void DestroyNodes(const std::vector<GlobalNodeRef>& aNodeRefs, CommandTracker* aCommandTracker);

		void SetNodePosition(NodeID aNodeID, Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void SetNodePosition(NodeID aNodeID, Vec2 aPosition, Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);


		std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, NodeID aNodeID, NodeTypeID aNodeTypeID, size_t aStartIndex = 0);

		std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, NodeID, NodeTypeID aNodeTypeID, size_t aStartIndex);

		PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID);
		PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID, void* aDataPtr);

		LinkID TryCreateLink(NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2, CommandTracker* aCommandTracker);
		LinkID CreateLink(NodeGraph& aNodeGraph, PinID aInputPinID, PinID aOutputPinID, CommandTracker* aCommandTracker);
		void DestroyLink(NodeGraph& aNodeGraph, LinkID aLinkID, CommandTracker* aCommandTracker);

		VarID CreateVariable(Class& aClass, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		void SetVariableDataType(Class& aClass, VarID aVarID, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		void DestroyVariableNodes(Class& aClass, VarID aVarID, CommandTracker* aCommandTracker);

		void BindVariable(Class& aClass, const NodeRef& aNodeRef, VarID aVarID, CommandTracker* aCommandTracker);
		void UnbindVariable(Class& aClass, const NodeRef& aNodeRef, CommandTracker* aCommandTracker);


		void ReplaceWildcardNode(NodeGraph& aNodeGraph, PinID aWildcardPinID, PinID aConnectedPinID, CommandTracker* aCommandTracker);
		void ReplaceWildcardNode(NodeGraph& aNodeGraph, PinID aWildcardPinID, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph);

		VariableRef GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef);
		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef);
	}



}