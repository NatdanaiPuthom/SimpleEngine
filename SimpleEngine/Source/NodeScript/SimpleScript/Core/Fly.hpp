#pragma once
#include "FlyDefines.hpp"
#include "SystemTypes/FlyVec2.hpp"
#include "Pin/FlyPin.hpp"
#include "Pin/FlyPinType.hpp"
#include "View/FlyNodeView.hpp"
#include "View/FlyVariableView.hpp"
#include "View/FlyLinkView.hpp"
#include "Utilities/FlyUtilities.hpp"
#include "Utilities/FlyMeta.hpp"
#include "View/FlyFunctionView.hpp"
#include "View/FlyCustomEventView.hpp"
#include "View/FlyDataTypeView.hpp"
#include "View/FlyClassView.hpp"
#include "View/FlyNodeGraphView.hpp"
#include "View/FlyClassInstanceView.hpp"
#include "SystemTypes/FlyNone.hpp"
#include "SystemTypes/FlyWildcard.hpp"
#include <unordered_map>

namespace FLY_NAMESPACE
{

	class CommandTracker;
	class ClassInstance;


	void SaveClass(ClassView aClassView, std::string_view aSavePath);
	void LoadAllClasses(std::string_view aFilePath);
	void SaveCustomEvents(std::string_view aFilePath);

	ClassView CreateClass(DataTypeView aTargetView, std::string_view aName, std::string_view aSavePath);
	ClassView CreateClassWithoutTarget(std::string_view aName, std::string_view aSavePath);
	void SetClassName(ClassView aClassView, std::string_view aName);

	ClassView FindClassByName(std::string_view aName);

	ClassInstanceView CreateClassInstance(ClassView aClassView);
	void DestroyClassInstance(ClassInstanceView aClassInstanceView);

	void EditClassInstanceVariableDefaultValue(ClassInstanceView aClassInstanceView);

	NodeView CreateNode(NodeGraphView aNodeGraphView, NodeTypeView aNodeTypeView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeView CreateNode(NodeGraphView aNodeGraphView, std::string_view aName, bool& aSuccess, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
	NodeView CreateNodeAutoLink(NodeGraphView aNodeGraph, NodeTypeView aNodeTypeView, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeView CreateGetterNode(NodeGraphView aNodeGraphView, VariableView aVariableView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeView CreateSetterNode(NodeGraphView aNodeGraphView, VariableView aVariableView, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

	LinkView TryCreateLink(PinView aPinView1, PinView aPinView2, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);
	void DestroyLink(LinkView aLinkView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);
	void DestroyLinksByPin(PinView aPinView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);
	void DestroyLinksByNode(NodeView aNodeView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);
	void DestroyNode(NodeView aNodeView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);
	void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);
	void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker);

	void SetNodePosition(NodeView aNodeView, Vec2 aPosition, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker = nullptr);
	void SetNodePosition(NodeView aNodeView, Vec2 aPosition, Vec2 aOldPosition, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);
	void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);

	VariableView CreateVariable(ClassView aClassView, DataTypeView aDataTypeView, CommandTracker* aCommandTracker = nullptr);
	void DestroyVariable(VariableView aVariableView, CommandTracker* aCommandTracker);
	void EditVariableDefaultValue(VariableView aVariableView, CommandTracker* aCommandTracker);
	void SetVariableDataType(VariableView aVariableView, DataTypeView aDataTypeView, CommandTracker* aCommandTracker);
	void SetVariableName(VariableView aVariableView, std::string_view aName, CommandTracker* aCommandTracker);
	void DestroyVariableNodes(VariableView aVariableView, ClassView aClassView, CommandTracker* aCommandTracker);

	void EditPin(PinView aPinView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);
	void SplitPin(PinView aPinView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker);

	bool HasPinAnyConnectedLinks(PinView aPinView);
	bool HasNodeAnyConnectedLinks(NodeView aNodeView, NodeGraphView aNodeGraphView);
	bool IsNodeReplacable(NodeView aNodeView, NodeGraphView aNodeGraphView);
	bool IsPinReplacable(PinView aPinView, NodeGraphView aNodeGraphView);
	void ReplaceTemplateNode(PinView aReplacedPinView, NodeGraphView aNodeGraphView, DataTypeView aDataTypeView, CommandTracker* aCommandTracker);
	void ReplaceTemplateNode(NodeView aReplacedNodeView, NodeGraphView aNodeGraphView, DataTypeView aDataTypeView, CommandTracker* aCommandTracker);
	std::vector<DataTypeView> GetReplacableDataTypes(PinView aPinView, NodeGraphView aNodeGraphView);
	std::vector<DataTypeView> GetReplacableDataTypes(NodeView aNodeView, NodeGraphView aNodeGraphView);

	void SetPinTypeName(PinTypeView aPinTypeView, std::string_view aName);

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, NodeGraphView aCopiedFromNodeGraphView);
	void PasteCopyBuffer(Vec2 aPosition, NodeGraphView aTargetNodeGraphView, CommandTracker* aCommandTracker);

	CustomEventView CreateCustomEvent(std::string_view aName);

	void AddPinToCustomEvent(CustomEventView aCustomEventView, DataTypeView aDataTypeView, std::string_view aPinName, CommandTracker* aCommandTracker);
	void SetPinDataTypeAtIndexCustomEvent(CustomEventView aCustomEventView, DataTypeView aDataTypeView, size_t aIndex, CommandTracker* aCommandTracker);
	void SetPinNameAtIndexCustomEvent(CustomEventView aCustomEventView, std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker);
	void DeletePinAtIndexCustomEvent(CustomEventView aCustomEventView, size_t aIndex, CommandTracker* aCommandTracker);

	void SetCustomEventName(CustomEventView aCustomEventView, std::string_view aName, CommandTracker* aCommandTracker);

	FunctionView CreateGlobalFunction(const std::string_view aName);
	FunctionView CreateMemberFunction(const std::string_view aName, ClassView aClassView);

	void AddPinToFunction(FunctionView aFunctionView, DataTypeView aDataTypeView, eFlowType aFlowType, std::string_view aPinName, CommandTracker* aCommandTracker);
	void SetPinDataTypeAtIndexFunction(FunctionView aFunctionView, DataTypeView aDataTypeView, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
	void SetPinNameAtIndexFunction(FunctionView aFunctionView, std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
	void DeletePinAtIndexFunction(FunctionView aFunctionView, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);

	void SetFunctionName(FunctionView aFunctionView, std::string_view aName, CommandTracker* aCommandTracker);

	void BeginFrame();

	std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<PinView> GetNonConnectedInputPinViews(NodeGraphView aNodeGraphView);
	std::vector<PinView> GetNonConnectedOutputPinViews(NodeGraphView aNodeGraphView);
	std::vector<PinView> GetNonConnectedPinViewsByFlowType(NodeGraphView aNodeGraphView, eFlowType aFlowType);
	std::vector<PinView> GetNonConnectedPinViewsByFlowTypeAndDataType(NodeGraphView aNodeGraphView, eFlowType aFlowType, DataTypeView aDataTypeView);

	VariableView GetVariableByNode(NodeView aNodeView, NodeGraphView aNodeGraphView);
	std::vector<VariableView> GetVariables(ClassView aClassView, bool aIncludeDestroyed = false);

	std::vector<NodeView> GetNodes(NodeGraphView aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<LinkView> GetLinks(NodeGraphView aNodeGraph, bool aIncludeDestroyed = false);

	std::vector<DataTypeView> GetDataTypes();

	template<Predicate<const DataTypeView&> FilterPredicate>
	std::vector<DataTypeView> GetDataTypesFiltered(FilterPredicate&& aFilterPredicate);

	std::vector<NodeTypeView> GetNodeTypes();

	std::vector<FunctionView> GetFunctions();
	std::vector<CustomEventView> GetCustomEvents();

	std::vector<NodeTypeView> GetNodeTypesFilteredByDataTypeAndFlowType(DataTypeID aDataTypeID, eFlowType aFlowType);
	std::vector<NodeTypeView> GetNodeTypesFilteredByTrait(eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait) = HasFlag);

	std::unordered_map<DataTypeView, std::vector<ClassView>> GetClasses();
	std::vector<ClassView> GetClassesByDataType(DataTypeView aDataTypeView);

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
