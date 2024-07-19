#pragma once
#include "ScriptDefines.h"
#include "SystemTypes/ScriptVec2.h"
#include "Pin/Pin.h"
#include "Pin/PinType.h"
#include "View/NodeView.h"
#include "View/VariableView.h"
#include "View/LinkView.h"
#include <unordered_map>
#include "Utilities/ScriptUtilities.h"
#include "View/FunctionView.h"
#include "View/CustomEventView.h"

namespace SCR
{

	struct NodeDragData
	{
		Vec2 startPos;
		Vec2 endPos;
	};

	class Script;
	class ScriptFoundation;
	class MoveNodesCommand;
	class CommandTracker;
	class Function;


	void SaveScript(const Script& aScript, std::string_view aSavePath);

	Script& CreateScript(DataTypeID aTarget, const std::string& aName);

	NodeID CreateNode(NodeGraph& aNodeGraph, NodeTypeID aNodeTypeID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeID CreateNodeAutoLink(NodeGraph& aNodeGraph, NodeTypeID aNodeTypeID, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeID CreateNode(NodeGraph& aNodeGraph, std::string_view aName, bool& aSuccess, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
	NodeID CreateGetterNode(Script& aScript, NodeGraph& aNodeGraph, VarID aVarID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeID CreateSetterNode(Script& aScript, NodeGraph& aNodeGraph, VarID aVarID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

	LinkID TryCreateLink(PinID aPinID1, PinID aPinID2, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroyLink(LinkID aLinkID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestoryLinksByOutputPinID(PinID aOutputPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroyNode(NodeID aNodeID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker);
	void SetNodePosition(NodeID aNodeID, Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker = nullptr);
	void SetNodePosition(NodeID aNodeID, Vec2 aPosition, Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	VarID CreateVariable(Script& aScript, DataTypeID aDataTypeID = typeid(bool).hash_code(), CommandTracker* aCommandTracker = nullptr);
	void DestroyVariable(VarID aVarID, Script& aScript, CommandTracker* aCommandTracker);

	void EditPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void SplitPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	void EditVariableDefaultValue(VarID aVarID, Script& aScript, CommandTracker* aCommandTracker);
	void SetVariableDataType(VarID aVarID, DataTypeID aDataTypeID, Script& aScript, CommandTracker* aCommandTracker);
	void SetVariableName(VarID aVarID, std::string_view aName, Script& aScript);

	void DestroyVariableNodes(VarID aVarID, Script& aScript, CommandTracker* aCommandTracker);

	void SetPinTypeName(PinTypeID aPinTypeID, std::string_view aName);

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraph& aNodeGraph);
	void PasteCopyBuffer(Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	CustomEventID CreateCustomEvent(std::string_view aName);

	void AddPinToCustomEvent(DataTypeID aDataTypeID, CustomEventID aCustomEventID, std::string_view aPinName = "Pin");
	void SetPinAtIndexCustomEvent(size_t anIndex, DataTypeID aDataTypeID, CustomEventID aCustomEventID);
	void DeletePinAtIndexCustomEvent(size_t anIndex, CustomEventID aCustomEventID);

	void SetCustomEventName(CustomEventID aCustomEventID, std::string_view aName);

	FunctionID CreateGlobalFunction(const std::string& aName);
	FunctionID CreateMemberFunction(const std::string& aName, Script& aScript);

	void AddPinToFunction(FunctionID aFunctionID, DataTypeID aDataTypeID, eFlowType aFlowType, std::string_view aPinName = "Pin");
	void SetPinAtIndexFunction(FunctionID aFunctionID, size_t anIndex, DataTypeID aDataTypeID, eFlowType aFlowType);
	void DeletePinAtIndexFunction(FunctionID aFunctionID, size_t anIndex, eFlowType aFlowType);

	void SetFunctionName(FunctionID aFunctionID, std::string_view aName);

	void BeginFrame();

	//int GetPinID(PinID aPinID, const NodeGraph& aNodeGraph);

	VariableView GetVariableByNodeID(NodeID aNodeID, NodeGraph& aNodeGraph, const Script& aScript);
	std::vector<VariableView> GetVariables(const Script& aScript, bool aIncludeDestroyed = false);

	std::vector<NodeView> GetNodes(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<LinkView> GetLinks(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);

	std::vector<NodeTypeView> GetNodeTypes();

	std::vector<FunctionView> GetFunctions();
	std::vector<CustomEventView> GetCustomEvents();

	std::vector<NodeTypeView> GetNodeTypesFilteredByDataTypeAndFlowType(DataTypeID aDataTypeID, eFlowType aFlowType);
	std::vector<NodeTypeView> GetNodeTypesFilteredByTrait(eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait) = HasFlag);

	const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Script>>>& GetScripts();
}
