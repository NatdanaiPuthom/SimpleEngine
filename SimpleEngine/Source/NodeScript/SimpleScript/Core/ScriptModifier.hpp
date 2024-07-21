#pragma once
#include "ScriptDefines.hpp"
#include "SystemTypes/ScriptVec2.hpp"
#include "Pin/Pin.hpp"
#include "Pin/PinType.hpp"
#include "View/NodeView.hpp"
#include "View/VariableView.hpp"
#include "View/LinkView.hpp"
#include "Utilities/ScriptUtilities.hpp"
#include "Utilities/MetaScript.hpp"
#include "View/FunctionView.hpp"
#include "View/CustomEventView.hpp"
#include "View/FlyDataTypeView.hpp"
#include "View/FlyClassView.hpp"
#include "View/FlyNodeGraphView.hpp"
#include "SystemTypes/FlyNone.hpp"
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


	void SaveClass(ClassView aClassView, std::string_view aSavePath);

	ClassView CreateClass(DataTypeView aTargetView, std::string_view aName);
	ClassView CreateClassWithoutTarget(std::string_view aName);
	void SetClassName(ClassView aClassView, std::string_view aName);

	NodeView CreateNode(NodeGraph& aNodeGraph, NodeTypeView aNodeTypeView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeView CreateNodeAutoLink(NodeGraph& aNodeGraph, NodeTypeView aNodeTypeView, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeView CreateNode(NodeGraph& aNodeGraph, std::string_view aName, bool& aSuccess, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
	NodeView CreateGetterNode(ClassView aClassView, NodeGraph& aNodeGraph, VarID aVarID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeView CreateSetterNode(ClassView aClassView, NodeGraph& aNodeGraph, VarID aVarID, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

	LinkView TryCreateLink(PinView aPinView1, PinView aPinView2, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroyLink(LinkID aLinkID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestoryLinksByOutputPin(PinView aOutputPinView, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroyNode(NodeView aNodeView, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker);
	void SetNodePosition(NodeID aNodeID, Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker = nullptr);
	void SetNodePosition(NodeID aNodeID, Vec2 aPosition, Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	VariableView CreateVariable(ClassView aClassView, DataTypeID aDataTypeID = typeid(bool).hash_code(), CommandTracker* aCommandTracker = nullptr);
	void DestroyVariable(VariableView aVariableView, ClassView aClassView, CommandTracker* aCommandTracker);

	void EditVariableDefaultValue(VariableView aVariableView, ClassView aClassView, CommandTracker* aCommandTracker);
	void SetVariableDataType(VariableView aVariableView, DataTypeView aDataTypeView, ClassView aClassView, CommandTracker* aCommandTracker);
	void SetVariableName(VariableView aVariableView, std::string_view aName, ClassView aClassView);
	void DestroyVariableNodes(VariableView aVariableView, ClassView aClassView, CommandTracker* aCommandTracker);

	void EditPin(PinView aPinView, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);
	void SplitPin(PinView aPinView, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	void SetPinTypeName(PinTypeView aPinTypeView, std::string_view aName);

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraph& aNodeGraph);
	void PasteCopyBuffer(Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker);

	CustomEventView CreateCustomEvent(std::string_view aName);

	void AddPinToCustomEvent(DataTypeID aDataTypeID, CustomEventID aCustomEventID, std::string_view aPinName = "Pin");
	void SetPinAtIndexCustomEvent(size_t anIndex, DataTypeView aDataTypeView, CustomEventID aCustomEventID);
	void DeletePinAtIndexCustomEvent(size_t anIndex, CustomEventID aCustomEventID);

	void SetCustomEventName(CustomEventID aCustomEventID, std::string_view aName);

	FunctionView CreateGlobalFunction(const std::string_view aName);
	FunctionView CreateMemberFunction(const std::string_view aName, ClassView aClassView);

	void AddPinToFunction(FunctionView aFunctionView, DataTypeID aDataTypeID, eFlowType aFlowType, std::string_view aPinName = "Pin");
	void SetPinAtIndexFunction(FunctionView aFunctionView, size_t anIndex, DataTypeID aDataTypeID, eFlowType aFlowType);
	void DeletePinAtIndexFunction(FunctionView aFunctionView, size_t anIndex, eFlowType aFlowType);

	void SetFunctionName(FunctionView aFunctionView, std::string_view aName);

	void BeginFrame();

	VariableView GetVariableByNode(NodeView aNodeView, NodeGraph& aNodeGraph, ClassView aClass);
	std::vector<VariableView> GetVariables(ClassView aClass, bool aIncludeDestroyed = false);

	std::vector<NodeView> GetNodes(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<LinkView> GetLinks(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);

	std::vector<DataTypeView> GetDataTypes();

	template<Predicate<const DataTypeView&> FilterPredicate>
	std::vector<DataTypeView> GetDataTypesFiltered(FilterPredicate&& aFilterPredicate);

	std::vector<NodeTypeView> GetNodeTypes();

	std::vector<FunctionView> GetFunctions();
	std::vector<CustomEventView> GetCustomEvents();

	std::vector<NodeTypeView> GetNodeTypesFilteredByDataTypeAndFlowType(DataTypeID aDataTypeID, eFlowType aFlowType);
	std::vector<NodeTypeView> GetNodeTypesFilteredByTrait(eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait) = HasFlag);

	std::unordered_map<DataTypeView, std::vector<ClassView>> GetClasses();

	template<Predicate<const DataTypeView&> FilterPredicate>
	std::vector<DataTypeView> GetDataTypesFiltered(FilterPredicate&& aFilterPredicate)
	{
		const std::vector<DataTypeView> dataTypes = GetDataTypes();

		std::vector<DataTypeView> filtered;
		filtered.reserve(dataTypes.size());

		for (const DataTypeView& dataType : dataTypes)
		{
			if (aFilterPredicate(dataType))
			{
				filtered.push_back(dataType);
			}
		}

		return filtered;
	}
}
