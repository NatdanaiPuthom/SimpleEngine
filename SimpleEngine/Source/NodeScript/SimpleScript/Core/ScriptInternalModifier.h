#pragma once
#include "ScriptDefines.h"
#include "Utilities/ScriptProxy.h"
#include "Pin/Pin.h"
#include "Node/NodeTrait.h"
#include <string>

namespace SCR
{

	class Script;
	
	class ScriptInternalModifier final
	{
		friend class Script;
		friend class ScriptLoader;
		friend class ScriptModifier;

		ScriptInternalModifier(Script& aScript);
		~ScriptInternalModifier();

	public:

#pragma region NodeType

		static CustomEventID CreateCustomEvent(const std::string& aName, ScriptFoundation& aFoundation);
		static FunctionID CreateFunction(const std::string& aName);

#pragma endregion

#pragma region Node

		NodeID CreateNode(const NodeTypeID aNodeTypeID);
		NodeID CreateNode(const std::string& aName, bool& aSuccess, bool aCreateIfNameNotFound);
		NodeID CreateGetterNode(const DataTypeID aDataTypeID);
		NodeID CreateSetterNode(const DataTypeID aDataTypeID);
		NodeID CreateOperatorNode(const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID);

		void AddNode(Node&& aNode, const NodeID aNodeID);

		void UpdateNodeTypeIDSize();

#pragma endregion

#pragma region Pin

		PinID CreateInputPin(const NodeID aNodeID, const PinTypeID aPinTypeID);
		std::vector<PinID> CreateInputPins(const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex = 0);

		PinID CreateOutputPin(const NodeID aNodeID, const PinTypeID aPinTypeID);
		std::vector<PinID> CreateOutputPins(const NodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex);

		PinID CreateInputPin(const NodeID aNodeID, const PinTypeID aPinTypeID, const MemoryPoolID aMemoryPoolID);
		PinID CreateOutputPin(const NodeID aNodeID, const PinTypeID aPinTypeID, const MemoryPoolID aMemoryPoolID);

		void RebindLink(const PinID aInputPinID, const PinID aNewOutputPinID);

#pragma endregion

#pragma region Variable

		void BindVariable(NodeID aNodeID, VarID aVarID);
		void UnbindVariable(NodeID aNodeID);


#pragma endregion


	private:

		Link ReplaceOperatorNode(PinID aUndefinedPinID, PinID aConnectedPinID);

		NodeID GetCurrentNodeID() const;

	public:

		template<typename DataType>
		void AddNodeState(const NodeID aNodeID);

		template<typename DataType>
		DataType& GetNodeState(NodeID aNodeID);

		Script& myScript;
	};

	template<typename DataType>
	inline void ScriptInternalModifier::AddNodeState(NodeID aNodeID)
	{
		ScriptProxy::GetNodeStateMap(myScript).emplace(aNodeID, DataType());
	}
	template<typename DataType>
	inline DataType& ScriptInternalModifier::GetNodeState(NodeID aNodeID)
	{
		return std::any_cast<DataType&>(ScriptProxy::GetNodeStateMap(myScript).at(aNodeID));
	}
}