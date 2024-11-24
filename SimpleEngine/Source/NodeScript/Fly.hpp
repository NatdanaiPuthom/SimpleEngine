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

	void InitializeFoundation();
	void DestroyFoundation();

	void LoadAllClasses(std::string_view aFilePath);
	void SaveCustomEvents(std::string_view aFilePath);

	ClassFacade CreateClass(DataTypeFacade aTargetFacade, std::string_view aName, std::string_view aSavePath);
	ClassFacade CreateClassWithoutTarget(std::string_view aName, std::string_view aSavePath);

	ClassFacade FindClassByName(std::string_view aName);

	void DestroyClassInstance(ClassInstanceFacade aClassInstanceFacade);

	void EditClassInstanceVariableDefaultValue(ClassInstanceFacade aClassInstanceFacade);

	void EditVariableDefaultValue(VariableFacade aVariableFacade, CommandTracker* aCommandTracker);

	bool HasPinAnyConnectedLinks(PinFacade aPinFacade);
	bool HasNodeAnyConnectedLinks(NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade);
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

	void AddPinToFunction(FunctionFacade aFunctionFacade, DataTypeFacade aDataTypeFacade, eFlowType aFlowType, std::string_view aPinName, CommandTracker* aCommandTracker);
	void SetPinDataTypeAtIndexFunction(FunctionFacade aFunctionFacade, DataTypeFacade aDataTypeFacade, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
	void SetPinNameAtIndexFunction(FunctionFacade aFunctionFacade, std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
	void DeletePinAtIndexFunction(FunctionFacade aFunctionFacade, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);

	void BeginFrame(CommandTracker* aCommandTracker);

	std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);
	std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed = false);

	VariableFacade GetVariableByNode(NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade);
	std::vector<VariableFacade> GetVariables(ClassFacade aClassFacade, bool aIncludeDestroyed = false);


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
