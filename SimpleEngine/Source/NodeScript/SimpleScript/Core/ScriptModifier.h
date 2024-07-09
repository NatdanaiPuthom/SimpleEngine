#pragma once
#include "ScriptDefines.h"
#include "SystemTypes/ScriptVec2.h"
#include "Pin/Pin.h"
#include "Pin/PinType.h"
#include <unordered_map>

namespace SCR
{

	struct NodeDragData
	{
		ScriptVec2 startPos;
		ScriptVec2 endPos;
	};

	class Script;
	class ScriptFoundation;
	class MoveNodesCommand;
	class CommandTracker;
	class Function;

	namespace Modify
	{

		Script& CreateScript(DataTypeID aTarget, const std::string& aName);

		NodeID CreateNode(NodeGraph& aNodeGraph, NodeTypeID aNodeTypeID, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateNodeAutoLink(NodeGraph& aNodeGraph, NodeTypeID aNodeTypeID, PinID aConnection, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateNode(NodeGraph& aNodeGraph, const std::string& aName, bool& aSuccess, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
		NodeID CreateGetterNode(Script& aScript, NodeGraph& aNodeGraph, DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateSetterNode(Script& aScript, NodeGraph& aNodeGraph, DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr);

		LinkID TryCreateLink(PinID aPinID1, PinID aPinID2, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void DestroyLink(LinkID aLinkID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void DestoryLinksByOutputPinID(PinID aOutputPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void DestroyNode(NodeID aNodeID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void SetNodePosition(NodeID aNodeID, ScriptVec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker = nullptr);
		void SetNodePosition(NodeID aNodeID, ScriptVec2 aPosition, ScriptVec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

		void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

		VarID CreateVariable(Script& aScript, DataTypeID aDataTypeID = typeid(bool).hash_code(), CommandTracker* aCommandTracker = nullptr);
		void DestroyVariable(VarID aVarID, Script& aScript, CommandTracker* aCommandTracker);

		void EditPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void SplitPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

		void EditVariableDefaultValue(VarID aVarID, Script& aScript, CommandTracker* aCommandTracker);
		void SetVariableDataType(VarID aVarID, DataTypeID aDataTypeID, Script& aScript, CommandTracker* aCommandTracker);
		void SetVariableName(VarID aVarID, const std::string& aName, Script& aScript);

		void DestroyVariableNodes(VarID aVarID, Script& aScript, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

		void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraph& aNodeGraph);
		void PasteCopyBuffer(ScriptVec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

		CustomEventID CreateCustomEvent(const std::string& aName);

		void AddPinToCustomEvent(DataTypeID aDataTypeID, CustomEventID aCustomEventID, const std::string& aPinName = "Pin");
		void SetPinAtIndexCustomEvent(size_t anIndex, DataTypeID aDataTypeID, CustomEventID aCustomEventID);
		void DeletePinAtIndexCustomEvent(size_t anIndex, CustomEventID aCustomEventID);

		FunctionID CreateGlobalFunction(const std::string& aName);
		FunctionID CreateMemberFunction(const std::string& aName, Script& aScript);

		void AddPinToFunction(FunctionID aFunctionID, DataTypeID aDataTypeID, eFlowType aFlowType, const std::string& aPinName = "Pin");
		void SetPinAtIndexFunction(FunctionID aFunctionID, size_t anIndex, DataTypeID aDataTypeID, eFlowType aFlowType);
		void DeletePinAtIndexFunction(FunctionID aFunctionID, size_t anIndex, eFlowType aFlowType);

		void BeginFrame();

		int GetPinID(PinID aPinID, const NodeGraph& aNodeGraph);
	}
}