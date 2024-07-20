#pragma once
#include "ScriptDefines.hpp"
#include "Pin/Pin.hpp"
#include "Node/NodeTrait.hpp"
#include "ScriptProxy.hpp"
#include <string>

namespace FLY_NAMESPACE
{

	class Class;


	struct NodeGraphContext
	{
		NodeGraph& mNodeGraph;
		Class& script;
	};

	namespace Internal
	{


		CustomEventID CreateCustomEvent(std::string_view aName);
		FunctionID CreateFunction(std::string_view aName);
		NodeID CreateNode(NodeGraph& aNodeGraph, NodeTypeID aNodeTypeID, CommandTracker* aCommandTracker);
		NodeID CreateNode(NodeGraph& aNodeGraph, std::string_view aName, bool& aSuccess, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker);
		NodeID CreateGetterNode(NodeGraph& aNodeGraph, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		NodeID CreateSetterNode(NodeGraph& aNodeGraph, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		NodeID CreateOperatorNode(NodeGraph& aNodeGraph, eNodeOperatorTrait aOperatorTrait, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, NodeID aNodeID, CommandTracker* aCommandTracker);

	
		std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, NodeID aNodeID, NodeTypeID aNodeTypeID, size_t aStartIndex = 0);

		std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, NodeID, NodeTypeID aNodeTypeID, size_t aStartIndex);

		PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID);
		PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID, void* aDataPtr);

		LinkID CreateLink(NodeGraph& aNodeGraph, PinID aInputPinID, PinID aOutputPinID, CommandTracker* aCommandTracker);
		void DestroyLink(NodeGraph& aNodeGraph, LinkID aLinkID, CommandTracker* aCommandTracker);

		VarID CreateVariable(Class& aScript, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		void BindVariable(Class& aScript, const NodeRef& aNodeRef, VarID aVarID, CommandTracker* aCommandTracker);
		void UnbindVariable(Class& aScript, const NodeRef& aNodeRef, CommandTracker* aCommandTracker);


		void ReplaceOperatorNode(NodeGraph& aNodeGraph, PinID aUndefinedPinID, PinID aConnectedPinID, CommandTracker* aCommandTracker);

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph);
	}



}