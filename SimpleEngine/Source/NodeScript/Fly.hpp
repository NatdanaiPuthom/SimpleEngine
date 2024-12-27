#pragma once
#include "FlyDefines.hpp"
#include "SystemTypes/FlyVec2.hpp"
#include "Utilities/FlyMeta.hpp"
#include "Utilities/FlyUtilities.hpp"
#include "Facade/FlyNodeFacade.hpp"
#include "Facade/FlyVariableFacade.hpp"
#include "Facade/FlyLinkFacade.hpp"
#include "Facade/FlyFunctionFacade.hpp"
#include "Facade/FlyCustomEventFacade.hpp"
#include "Facade/FlyDataTypeFacade.hpp"
#include "Facade/FlyClassFacade.hpp"
#include "Facade/FlyStructFacade.hpp"
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

	void LoadAllFlyFiles(std::string_view aFilePath);
	void SaveCustomEvents(std::string_view aFilePath);

	StructFacade CreateStruct(std::string_view aName, std::string_view aSavePath);
	ClassFacade CreateClass(DataTypeFacade aTargetFacade, std::string_view aName, std::string_view aSavePath);
	ClassFacade CreateClassWithoutTarget(std::string_view aName, std::string_view aSavePath);

	[[nodiscard]] StructFacade FindStructByName(std::string_view aName);
	[[nodiscard]] ClassFacade FindClassByName(std::string_view aName);

	void SetDefaultDataTypeColor(const Fly::Color& aColor);
	void SetEditorNullptrFunction(void(*aFunction)());

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, NodeGraphFacade aCopiedFromNodeGraphFacade);
	void PasteCopyBuffer(Vec2 aPosition, NodeGraphFacade aTargetNodeGraphFacade, CommandTracker* aCommandTracker);

	CustomEventFacade CreateCustomEvent(std::string_view aName);
	FunctionFacade CreateGlobalFunction(const std::string_view aName);

	void BeginFrame(CommandTracker* aCommandTracker);
	[[nodiscard]] bool& IsDebugging();

	[[nodiscard]] std::vector<DataTypeFacade> GetDataTypes();
	[[nodiscard]] DataTypeFacade GetDataTypeFacadeByName(std::string_view aName);

	template<Predicate<const DataTypeFacade&> FilterPredicate>
	[[nodiscard]] std::vector<DataTypeFacade> GetDataTypesFiltered(FilterPredicate&& aFilterPredicate);

	[[nodiscard]] std::vector<NodeTypeFacade> GetNodeTypes();

	[[nodiscard]] std::vector<FunctionFacade> GetFunctions();
	[[nodiscard]] std::vector<CustomEventFacade> GetCustomEvents();

	[[nodiscard]] std::vector<LinkFacade> GetTraversedLinks();

	[[nodiscard]] std::vector<NodeTypeFacade> GetNodeTypesFilteredByRelatedDataTypesAndFlowTypeAndTrait(DataTypeID aDataTypeID, eFlowType aFlowType, eNodeTrait aNodeTrait, bool(*)(eNodeTrait, eNodeTrait));
	[[nodiscard]] std::vector<NodeTypeFacade> GetNodeTypesFilteredByTrait(eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait) = HasFlag);

	[[nodiscard]] std::unordered_map<DataTypeFacade, std::vector<ClassFacade>> GetClasses();
	[[nodiscard]] std::vector<ClassFacade> GetClassesByTargetDataType(DataTypeFacade aDataTypeFacade);

	template<Predicate<const DataTypeFacade&> FilterPredicate>
	[[nodiscard]] std::vector<DataTypeFacade> GetDataTypesFiltered(FilterPredicate&& aFilterPredicate)
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
