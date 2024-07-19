#pragma once
#include "ScriptDefines.h"
#include "Pin/Pin.h"
#include "Node/NodeTrait.h"
#include "ScriptProxy.h"
#include <string>

namespace SCR
{

	class Script;


	struct NodeGraphContext
	{
		NodeGraph& nodeGraph;
		Script& script;
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

		VarID CreateVariable(Script& aScript, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		void BindVariable(Script& aScript, const NodeRef& aNodeRef, VarID aVarID, CommandTracker* aCommandTracker);
		void UnbindVariable(Script& aScript, const NodeRef& aNodeRef, CommandTracker* aCommandTracker);


		void ReplaceOperatorNode(NodeGraph& aNodeGraph, PinID aUndefinedPinID, PinID aConnectedPinID, CommandTracker* aCommandTracker);

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph);
	}



}