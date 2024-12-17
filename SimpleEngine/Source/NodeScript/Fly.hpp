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
#include "Facade/FlyFacade.hpp"
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

	void SetDefaultDataTypeColor(const Fly::Color& aColor);
	void SetEditorNullptrFunction(void(*aFunction)());

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, NodeGraphFacade aCopiedFromNodeGraphFacade);
	void PasteCopyBuffer(Vec2 aPosition, NodeGraphFacade aTargetNodeGraphFacade, CommandTracker* aCommandTracker);

	CustomEventFacade CreateCustomEvent(std::string_view aName);
	FunctionFacade CreateGlobalFunction(const std::string_view aName);

	void BeginFrame(CommandTracker* aCommandTracker);

	std::vector<DataTypeFacade> GetDataTypes();

	template<Predicate<const DataTypeFacade&> FilterPredicate>
	std::vector<DataTypeFacade> GetDataTypesFiltered(FilterPredicate&& aFilterPredicate);

	std::vector<NodeTypeFacade> GetNodeTypes();

	std::vector<FunctionFacade> GetFunctions();
	std::vector<CustomEventFacade> GetCustomEvents();

	std::vector<LinkFacade> GetTraversedLinks();

	std::vector<NodeTypeFacade> GetNodeTypesFilteredByDataTypeAndFlowType(DataTypeID aDataTypeID, eFlowType aFlowType);
	std::vector<NodeTypeFacade> GetNodeTypesFilteredByRelatedDataTypesAndFlowTypeAndTrait(DataTypeID aDataTypeID, eFlowType aFlowType, eNodeTrait aNodeTrait, bool(*)(eNodeTrait, eNodeTrait));
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
