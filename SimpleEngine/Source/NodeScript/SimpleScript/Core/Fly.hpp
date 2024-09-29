#pragma once
#include "FlyDefines.hpp"
#include "SystemTypes/FlyVec2.hpp"
#include "Pin/FlyPin.hpp"
#include "Pin/FlyPinType.hpp"
#include "Facade/FlyNodeFacade.hpp"
#include "Facade/FlyVariableFacade.hpp"
#include "Facade/FlyLinkFacade.hpp"
#include "Utilities/FlyUtilities.hpp"
#include "Utilities/FlyMeta.hpp"
#include "Facade/FlyFunctionFacade.hpp"
#include "Facade/FlyCustomEventFacade.hpp"
#include "Facade/FlyDataTypeFacade.hpp"
#include "Facade/FlyClassFacade.hpp"
#include "Facade/FlyNodeGraphFacade.hpp"
#include "Facade/FlyClassInstanceFacade.hpp"
#include "SystemTypes/FlyNone.hpp"
#include "SystemTypes/FlyWildcard.hpp"
#include <unordered_map>

namespace FLY_NAMESPACE
{

	class CommandTracker;
	class ClassInstance;


	void SaveClass(ClassFacade aClassFacade, std::string_view aSavePath);
	void LoadAllClasses(std::string_view aFilePath);
	void SaveCustomEvents(std::string_view aFilePath);

	ClassFacade CreateClass(DataTypeFacade aTargetFacade, std::string_view aName, std::string_view aSavePath);
	ClassFacade CreateClassWithoutTarget(std::string_view aName, std::string_view aSavePath);
	void SetClassName(ClassFacade aClassFacade, std::string_view aName);

	ClassFacade FindClassByName(std::string_view aName);

	ClassInstanceFacade CreateClassInstance(ClassFacade aClassFacade);
	void DestroyClassInstance(ClassInstanceFacade aClassInstanceFacade);

	void EditClassInstanceVariableDefaultValue(ClassInstanceFacade aClassInstanceFacade);

	NodeFacade CreateNode(NodeGraphFacade aNodeGraphFacade, NodeTypeFacade aNodeTypeFacade, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeFacade CreateNode(NodeGraphFacade aNodeGraphFacade, std::string_view aName, bool& aSuccess, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr, bool aCreateIfNameNotFound = true);
	NodeFacade CreateNodeAutoLink(NodeGraphFacade aNodeGraph, NodeTypeFacade aNodeTypeFacade, PinID aConnection, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeFacade CreateGetterNode(NodeGraphFacade aNodeGraphFacade, VariableFacade aVariableFacade, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);
	NodeFacade CreateSetterNode(NodeGraphFacade aNodeGraphFacade, VariableFacade aVariableFacade, Vec2 aPosition = Vec2(), CommandTracker* aCommandTracker = nullptr);

	LinkFacade TryCreateLink(PinFacade aPinFacade1, PinFacade aPinFacade2, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);
	void DestroyLink(LinkFacade aLinkFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);
	void DestroyLinksByPin(PinFacade aPinFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);
	void DestroyLinksByNode(NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);
	void DestroyNode(NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);
	void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);
	void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker);

	void SetNodePosition(NodeFacade aNodeFacade, Vec2 aPosition, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker = nullptr);
	void SetNodePosition(NodeFacade aNodeFacade, Vec2 aPosition, Vec2 aOldPosition, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);
	void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);

	VariableFacade CreateVariable(ClassFacade aClassFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker = nullptr);
	void DestroyVariable(VariableFacade aVariableFacade, CommandTracker* aCommandTracker);
	void EditVariableDefaultValue(VariableFacade aVariableFacade, CommandTracker* aCommandTracker);
	void SetVariableDataType(VariableFacade aVariableFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker);
	void SetVariableName(VariableFacade aVariableFacade, std::string_view aName, CommandTracker* aCommandTracker);
	void DestroyVariableNodes(VariableFacade aVariableFacade, ClassFacade aClassFacade, CommandTracker* aCommandTracker);

	void EditPin(PinFacade aPinFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);
	void SplitPin(PinFacade aPinFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker);

	bool HasPinAnyConnectedLinks(PinFacade aPinFacade);
	bool HasNodeAnyConnectedLinks(NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade);
	bool IsNodeReplacable(NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade);
	bool IsPinReplacable(PinFacade aPinFacade, NodeGraphFacade aNodeGraphFacade);
	void ReplaceTemplateNode(PinFacade aReplacedPinFacade, NodeGraphFacade aNodeGraphFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker);
	void ReplaceTemplateNode(NodeFacade aReplacedNodeFacade, NodeGraphFacade aNodeGraphFacade, DataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker);
	std::vector<DataTypeFacade> GetReplacableDataTypes(PinFacade aPinFacade, NodeGraphFacade aNodeGraphFacade);
	std::vector<DataTypeFacade> GetReplacableDataTypes(NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade);

	void SetPinTypeName(PinTypeFacade aPinTypeFacade, std::string_view aName);

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, NodeGraphFacade aCopiedFromNodeGraphFacade);
	void PasteCopyBuffer(Vec2 aPosition, NodeGraphFacade aTargetNodeGraphFacade, CommandTracker* aCommandTracker);

	CustomEventFacade CreateCustomEvent(std::string_view aName);

	void AddPinToCustomEvent(CustomEventFacade aCustomEventFacade, DataTypeFacade aDataTypeFacade, std::string_view aPinName, CommandTracker* aCommandTracker);
	void SetPinDataTypeAtIndexCustomEvent(CustomEventFacade aCustomEventFacade, DataTypeFacade aDataTypeFacade, size_t aIndex, CommandTracker* aCommandTracker);
	void SetPinNameAtIndexCustomEvent(CustomEventFacade aCustomEventFacade, std::string_view aName, size_t aIndex, CommandTracker* aCommandTracker);
	void DeletePinAtIndexCustomEvent(CustomEventFacade aCustomEventFacade, size_t aIndex, CommandTracker* aCommandTracker);

	void SetCustomEventName(CustomEventFacade aCustomEventFacade, std::string_view aName, CommandTracker* aCommandTracker);

	FunctionFacade CreateGlobalFunction(const std::string_view aName);
	FunctionFacade CreateMemberFunction(const std::string_view aName, ClassFacade aClassFacade);

	void AddPinToFunction(FunctionFacade aFunctionFacade, DataTypeFacade aDataTypeFacade, eFlowType aFlowType, std::string_view aPinName, CommandTracker* aCommandTracker);
	void SetPinDataTypeAtIndexFunction(FunctionFacade aFunctionFacade, DataTypeFacade aDataTypeFacade, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
	void SetPinNameAtIndexFunction(FunctionFacade aFunctionFacade, std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
	void DeletePinAtIndexFunction(FunctionFacade aFunctionFacade, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);

	void SetFunctionName(FunctionFacade aFunctionFacade, std::string_view aName, CommandTracker* aCommandTracker);

	void BeginFrame();

	std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<PinFacade> GetNonConnectedInputPinFacades(NodeGraphFacade aNodeGraphFacade);
	std::vector<PinFacade> GetNonConnectedOutputPinFacades(NodeGraphFacade aNodeGraphFacade);
	std::vector<PinFacade> GetNonConnectedPinFacadesByFlowType(NodeGraphFacade aNodeGraphFacade, eFlowType aFlowType);
	std::vector<PinFacade> GetNonConnectedPinFacadesByFlowTypeAndDataType(NodeGraphFacade aNodeGraphFacade, eFlowType aFlowType, DataTypeFacade aDataTypeFacade);

	VariableFacade GetVariableByNode(NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade);
	std::vector<VariableFacade> GetVariables(ClassFacade aClassFacade, bool aIncludeDestroyed = false);

	std::vector<NodeFacade> GetNodes(NodeGraphFacade aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<LinkFacade> GetLinks(NodeGraphFacade aNodeGraph, bool aIncludeDestroyed = false);

	std::vector<DataTypeFacade> GetDataTypes();

	template<Predicate<const DataTypeFacade&> FilterPredicate>
	std::vector<DataTypeFacade> GetDataTypesFiltered(FilterPredicate&& aFilterPredicate);

	std::vector<NodeTypeFacade> GetNodeTypes();

	std::vector<FunctionFacade> GetFunctions();
	std::vector<CustomEventFacade> GetCustomEvents();

	std::vector<NodeTypeFacade> GetNodeTypesFilteredByDataTypeAndFlowType(DataTypeID aDataTypeID, eFlowType aFlowType);
	std::vector<NodeTypeFacade> GetNodeTypesFilteredByTrait(eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait) = HasFlag);

	std::unordered_map<DataTypeFacade, std::vector<ClassFacade>> GetClasses();
	std::vector<ClassFacade> GetClassesByDataType(DataTypeFacade aDataTypeFacade);

	template<Predicate<const DataTypeFacade&> FilterPredicate>
	std::vector<DataTypeFacade> GetDataTypesFiltered(FilterPredicate&& aFilterPredicate)
	{
		const std::vector<DataTypeFacade> dataTypes = GetDataTypes();

		std::vector<DataTypeFacade> filtered;
		filtered.reserve(dataTypes.size());

		for (const DataTypeFacade& dataType : dataTypes)
		{
			if (aFilterPredicate(dataType))
			{
				filtered.push_back(dataType);
			}
		}

		return filtered;
	}
}
