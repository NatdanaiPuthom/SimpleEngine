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
	
	class InternalModifier final
	{
		friend class Script;
		friend class ScriptLoader;
		friend class ScriptModifier;

		InternalModifier() = delete;
		~InternalModifier() = delete;

	public:

#pragma region NodeType

		static CustomEventID CreateCustomEvent(std::string_view aName);
		static FunctionID CreateFunction(std::string_view aName);

#pragma endregion

#pragma region Node

		static NodeID CreateNode(NodeGraph& aNodeGraph, NodeTypeID aNodeTypeID, CommandTracker* aCommandTracker);
		static NodeID CreateNode(NodeGraph& aNodeGraph, std::string_view aName, bool& aSuccess, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker);
		static NodeID CreateGetterNode(NodeGraph& aNodeGraph, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		static NodeID CreateSetterNode(NodeGraph& aNodeGraph, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		static NodeID CreateOperatorNode(NodeGraph& aNodeGraph, eNodeOperatorTrait aOperatorTrait, DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

	private:
		static void AddNode(NodeGraph& aNodeGraph, Node&& aNode, NodeID aNodeID, CommandTracker* aCommandTracker);

	public:

#pragma endregion

#pragma region Pin

		static std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, NodeID aNodeID, NodeTypeID aNodeTypeID, size_t aStartIndex = 0);

		static std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, NodeID, NodeTypeID aNodeTypeID, size_t aStartIndex);

		static PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID);
		static PinID CreatePin(NodeGraph& aNodeGraph, NodeID aNodeID, PinTypeID aPinTypeID, void* aDataPtr);

		static LinkID CreateLink(NodeGraph& aNodeGraph, PinID aInputPinID, PinID aOutputPinID, CommandTracker* aCommandTracker);
		static void DestroyLink(NodeGraph& aNodeGraph, LinkID aLinkID, CommandTracker* aCommandTracker);

#pragma endregion

#pragma region Variable

		static void BindVariable(Script& aScript, const NodeRef& aNodeRef, VarID aVarID, CommandTracker* aCommandTracker);
		static void UnbindVariable(Script& aScript, const NodeRef& aNodeRef, CommandTracker* aCommandTracker);


#pragma endregion


	public:

		static void ReplaceOperatorNode(NodeGraph& aNodeGraph, PinID aUndefinedPinID, PinID aConnectedPinID, CommandTracker* aCommandTracker);

		static NodeID GetCurrentNodeID(NodeGraph& aNodeGraph);
	};


}