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

		static CustomEventID CreateCustomEvent(const std::string& aName);
		static FunctionID CreateFunction(const std::string& aName);

#pragma endregion

#pragma region Node

		static NodeID CreateNode(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, CommandTracker* aCommandTracker);
		static NodeID CreateNode(NodeGraph& aNodeGraph, const std::string& aName, bool& aSuccess, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker);
		static NodeID CreateGetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		static NodeID CreateSetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker);
		static NodeID CreateOperatorNode(NodeGraph& aNodeGraph, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker);

		static void AddNode(NodeGraph& aNodeGraph, Node&& aNode, const NodeID aNodeID, CommandTracker* aCommandTracker);

		static void UpdateNodeTypeIDSize(NodeGraph& aNodeGraph);

#pragma endregion

#pragma region Pin

		static PinID CreateInputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID);
		static std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex = 0);

		static PinID CreateOutputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID);
		static std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, const NodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex);

		static PinID CreateInputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, void* aDataPtr);
		static PinID CreateOutputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, void* aDataPtr);

		//static PinID CreateInputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, const MemoryPoolID aMemoryPoolID);
		//static PinID CreateOutputPin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, const MemoryPoolID aMemoryPoolID);

		static void RebindLink(NodeGraph& aNodeGraph, const PinID aInputPinID, const PinID aNewOutputPinID, CommandTracker* aCommandTracker);

#pragma endregion

#pragma region Variable

		static void BindVariable(Script& aScript, NodeID aNodeID, VarID aVarID);
		static void UnbindVariable(Script& aScript, NodeID aNodeID);


#pragma endregion


	private:

		static Link ReplaceOperatorNode(const NodeGraphContext& aContext, PinID aUndefinedPinID, PinID aConnectedPinID);

		static NodeID GetCurrentNodeID(NodeGraph& aNodeGraph);

	public:

		template<typename DataType>
		static void AddNodeState(NodeGraph& aNodeGraph, const NodeID aNodeID);
	};

	template<typename DataType>
	inline void InternalModifier::AddNodeState(NodeGraph& aNodeGraph, NodeID aNodeID)
	{
		ScriptProxy::GetNodeStateMap(aNodeGraph).emplace(aNodeID, DataType());
	}
}