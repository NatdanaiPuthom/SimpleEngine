#pragma once
#include "ScriptDefines.h"
#include "SystemTypes/ScriptVec2.h"
#include "Pin/Pin.h"

namespace SCR
{
	
	struct MoveNodeData
	{
		ScriptVec2 startPos, endPos;
	};

	struct NodeDragData
	{
		NodeID nodeID;
		ScriptVec2 pos;
	};

	class Script;
	class ScriptFoundation;
	class MoveNodesCommand;
	class CommandTracker;

	namespace Modify
	{
		NodeID CreateNode(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateNodeAutoLink(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, PinID aConnection, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateNode(NodeGraph& aNodeGraph, const std::string& aName, bool& aSuccess, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
		NodeID CreateGetterNode(Script& aScript, NodeGraph& aNodeGraph, DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr);
		NodeID CreateSetterNode(Script& aScript, NodeGraph& aNodeGraph, DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition = ScriptVec2(), CommandTracker* aCommandTracker = nullptr);

		Link TryCreateLink(NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2, CommandTracker* aCommandTracker);
		void DestroyLink(const PinID aInputPinID);
		void DestoryLinksByOutputPinID(const PinID aOutputPinID);
		void DestroyNode(const NodeID aNodeID);
		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs);
		void SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
		void SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, ScriptVec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

		void BeginNodeDrag(const std::vector<NodeDragData>& aDragData);
		void EndNodeDrag(const std::vector<NodeDragData>& aDragData);
		void BeginNodeDrag(const NodeID aNodeID, ScriptVec2 aStartPos);
		void EndNodeDrag(const NodeID aNodeID, ScriptVec2 aEndPos);

		VarID CreateVariable(DataTypeID aDataTypeID = typeid(bool).hash_code());
		void DestroyVariable(VarID aVarID);

		void EditPin(PinID anPinID);
		void EditVariableDefaultValue(VarID aVarID);
		void SetVariableDataType(VarID aVarID, DataTypeID aDataTypeID);
		void SetVariableName(VarID aVarID, const std::string& aName);

		void DestroyVariableNodes(const VarID aVarID);

		void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs);
		void PasteCopyBuffer(ScriptVec2 aPosition);

		static CustomEventID CreateNodeType_CustomEvent(const std::string& aName);

		static void AddPinToCustomEvent(const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID, const std::string& aName = "Pin", ScriptFoundation* aFoundation = nullptr);
		static void SetPinAtIndexCustomEvent(const size_t anIndex, const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID, ScriptFoundation* aFoundation = nullptr);
		static void DeletePinAtIndexCustomEvent(const size_t anIndex, const CustomEventID aNodeTypeID, ScriptFoundation* aFoundation = nullptr);

		static FunctionID CreateFunction(const std::string& aName);
	}

	class ScriptModifier
	{
		friend class Script;

		ScriptModifier(Script& aScript);
		~ScriptModifier();
	public:


		NodeID CreateNode(const NodeTypeID aNodeTypeID, ScriptVec2 aPosition = ScriptVec2());
		NodeID CreateNodeAutoLink(const NodeTypeID aNodeTypeID, PinID aConnection, ScriptVec2 aPosition = ScriptVec2());
		NodeID CreateNode(const std::string& aName, bool& aSuccess, ScriptVec2 aPosition = ScriptVec2(), bool aTrackChange = false, bool aCreateIfNameNotFound = true);
		NodeID CreateGetterNode(DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition = ScriptVec2());
		NodeID CreateSetterNode(DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition = ScriptVec2());

		Link TryCreateLink(PinID aPinID1, PinID aPinID2);
		void DestroyLink(const PinID aInputPinID);
		void DestoryLinksByOutputPinID(const PinID aOutputPinID);
		void DestroyNode(const NodeID aNodeID);
		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs);
		void SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, bool aTrackChange = false);
		void SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, ScriptVec2 aOldPosition, bool aTrackChange = false);

		void BeginNodeDrag(const std::vector<NodeDragData>& aDragData);
		void EndNodeDrag(const std::vector<NodeDragData>& aDragData);
		void BeginNodeDrag(const NodeID aNodeID, ScriptVec2 aStartPos);
		void EndNodeDrag(const NodeID aNodeID, ScriptVec2 aEndPos);

		VarID CreateVariable(DataTypeID aDataTypeID = typeid(bool).hash_code());
		void DestroyVariable(VarID aVarID);

		void EditPin(PinID anPinID);
		void EditVariableDefaultValue(VarID aVarID);
		void SetVariableDataType(VarID aVarID, DataTypeID aDataTypeID);
		void SetVariableName(VarID aVarID, const std::string& aName);

		void DestroyVariableNodes(const VarID aVarID);

		void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs);
		void PasteCopyBuffer(ScriptVec2 aPosition);

		static CustomEventID CreateNodeType_CustomEvent(const std::string& aName);

		static void AddPinToCustomEvent(const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID, const std::string& aName = "Pin", ScriptFoundation* aFoundation = nullptr);
		static void SetPinAtIndexCustomEvent(const size_t anIndex, const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID, ScriptFoundation* aFoundation = nullptr);
		static void DeletePinAtIndexCustomEvent(const size_t anIndex, const CustomEventID aNodeTypeID, ScriptFoundation* aFoundation = nullptr);

		static FunctionID CreateFunction(const std::string& aName);

		void SetCurrentNodeGraph(NodeGraph* aNodeGraph);
		NodeGraph* GetCurrentNodeGraph() const;

	private:

		Script& myScript;
		NodeGraph* myCurrentNodeGraph;

		std::unordered_map<NodeID, MoveNodeData> myMoveNodesData;
	};

	
}