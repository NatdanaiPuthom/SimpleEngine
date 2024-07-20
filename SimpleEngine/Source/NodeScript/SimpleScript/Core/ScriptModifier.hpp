#pragma once
#include "ScriptDefines.hpp"
#include "SystemTypes/ScriptVec2.hpp"
#include "Pin/Pin.hpp"
#include "Pin/PinType.hpp"
#include "View/NodeView.hpp"
#include "View/VariableView.hpp"
#include "View/LinkView.hpp"
#include "Utilities/ScriptUtilities.hpp"
#include "View/FunctionView.hpp"
#include "View/CustomEventView.hpp"
#include <unordered_map>

namespace FLY_NAMESPACE
{

	struct NodeDragData
	{
		Vec2 startPos;
		Vec2 endPos;
	};

	class Class;
	class ScriptFoundation;
	class MoveNodesCommand;
	class CommandTracker;
	class Function;


	void SaveClass(const Class& aClass, std::string_view aSavePath);

	Class& CreateClass(DataTypeID aTarget, const std::string& aName);

	NodeView CreateNode(NodeGraph& aNodeGraph, NodeTypeID aNodeTypeID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeView CreateNodeAutoLink(NodeGraph& aNodeGraph, NodeTypeID aNodeTypeID, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeView CreateNode(NodeGraph& aNodeGraph, std::string_view aName, bool& aSuccess, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
	NodeView CreateGetterNode(Class& aScript, NodeGraph& aNodeGraph, VarID aVarID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeView CreateSetterNode(Class& aScript, NodeGraph& aNodeGraph, VarID aVarID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

	LinkID TryCreateLink(PinID aPinID1, PinID aPinID2, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroyLink(LinkID aLinkID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestoryLinksByOutputPinID(PinID aOutputPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroyNode(NodeID aNodeID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker);
	void SetNodePosition(NodeID aNodeID, Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker = nullptr);
	void SetNodePosition(NodeID aNodeID, Vec2 aPosition, Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	VariableView CreateVariable(Class& aScript, DataTypeID aDataTypeID = typeid(bool).hash_code(), CommandTracker* aCommandTracker = nullptr);
	void DestroyVariable(VarID aVarID, Class& aScript, CommandTracker* aCommandTracker);

	void EditPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void SplitPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	void EditVariableDefaultValue(VarID aVarID, Class& aScript, CommandTracker* aCommandTracker);
	void SetVariableDataType(VarID aVarID, DataTypeID aDataTypeID, Class& aScript, CommandTracker* aCommandTracker);
	void SetVariableName(VarID aVarID, std::string_view aName, Class& aScript);

	void DestroyVariableNodes(VarID aVarID, Class& aScript, CommandTracker* aCommandTracker);

	void SetPinTypeName(PinTypeID aPinTypeID, std::string_view aName);

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraph& aNodeGraph);
	void PasteCopyBuffer(Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	CustomEventView CreateCustomEvent(std::string_view aName);

	void AddPinToCustomEvent(DataTypeID aDataTypeID, CustomEventID aCustomEventID, std::string_view aPinName = "Pin");
	void SetPinAtIndexCustomEvent(size_t anIndex, DataTypeID aDataTypeID, CustomEventID aCustomEventID);
	void DeletePinAtIndexCustomEvent(size_t anIndex, CustomEventID aCustomEventID);

	void SetCustomEventName(CustomEventID aCustomEventID, std::string_view aName);

	FunctionView CreateGlobalFunction(const std::string& aName);
	FunctionView CreateMemberFunction(const std::string& aName, Class& aScript);

	void AddPinToFunction(FunctionID aFunctionID, DataTypeID aDataTypeID, eFlowType aFlowType, std::string_view aPinName = "Pin");
	void SetPinAtIndexFunction(FunctionID aFunctionID, size_t anIndex, DataTypeID aDataTypeID, eFlowType aFlowType);
	void DeletePinAtIndexFunction(FunctionID aFunctionID, size_t anIndex, eFlowType aFlowType);

	void SetFunctionName(FunctionID aFunctionID, std::string_view aName);

	void BeginFrame();

	//int GetPinID(PinID aPinID, const NodeGraph& aNodeGraph);

	VariableView GetVariableByNodeID(NodeID aNodeID, NodeGraph& aNodeGraph, const Class& aScript);
	std::vector<VariableView> GetVariables(const Class& aScript, bool aIncludeDestroyed = false);

	std::vector<NodeView> GetNodes(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<LinkView> GetLinks(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);

	std::vector<NodeTypeView> GetNodeTypes();

	std::vector<FunctionView> GetFunctions();
	std::vector<CustomEventView> GetCustomEvents();

	std::vector<NodeTypeView> GetNodeTypesFilteredByDataTypeAndFlowType(DataTypeID aDataTypeID, eFlowType aFlowType);
	std::vector<NodeTypeView> GetNodeTypesFilteredByTrait(eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait) = HasFlag);

	const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>>& GetScripts();
}
