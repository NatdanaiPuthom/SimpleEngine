#pragma once
#include "FlyDefines.hpp"
#include "SystemTypes/FlyVec2.hpp"
#include "Utilities/FlyMeta.hpp"
#include "Utilities/FlyUtilities.hpp"
#include "Proxy/FlyNodeProxy.hpp"
#include "Proxy/FlyVariableProxy.hpp"
#include "Proxy/FlyLinkProxy.hpp"
#include "Proxy/FlyFunctionProxy.hpp"
#include "Proxy/FlyCustomEventProxy.hpp"
#include "Proxy/FlyDataTypeProxy.hpp"
#include "Proxy/FlyStructProxy.hpp"
#include "Proxy/FlyClassProxy.hpp"
#include "Proxy/FlyGenericDataTypeProxy.hpp"
#include "Proxy/FlyNodeGraphProxy.hpp"
#include "Proxy/FlyClassInstanceProxy.hpp"
#include "Proxy/FlyProxy.hpp"
#include "SystemTypes/FlyNone.hpp"
#include "SystemTypes/FlyWildcard.hpp"
#include "Proxy/ProxyIterators/FlyDataTypeProxyIterator.hpp"
#include <unordered_map>

namespace FLY_NAMESPACE
{

	class CommandTracker;

	void InitializeFoundation();
	void DestroyFoundation();

	void LoadAllFlyFiles(std::string_view aFilePath);
	void SaveCustomEvents(std::string_view aFilePath);

	GenericDataTypeProxy CreateStruct(std::string_view aName, std::string_view aSavePath);
	ClassProxy CreateClass(GenericDataTypeProxy aTargetProxy, std::string_view aName, std::string_view aSavePath);
	ClassProxy CreateClassWithoutTarget(std::string_view aName, std::string_view aSavePath);

	[[nodiscard]] GenericDataTypeProxy FindDataTypeByName(std::string_view aName);
	[[nodiscard]] ClassProxy FindClassByName(std::string_view aName);

	void SetDefaultDataTypeColor(const Fly::Color& aColor);
	void SetEditorTextFunction(void(*aTextFunction)(const std::string&));

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, NodeGraphProxy aCopiedFromNodeGraphProxy);
	void PasteCopyBuffer(Vec2 aPosition, NodeGraphProxy aTargetNodeGraphProxy, CommandTracker* aCommandTracker);

	CustomEventProxy CreateCustomEvent(std::string_view aName);
	FunctionProxy CreateGlobalFunction(std::string_view aName);

	void BeginFrame(CommandTracker* aCommandTracker);
	[[nodiscard]] bool& IsDebugging();

	[[nodiscard]] std::vector<DataTypeProxy> GetDataTypes();
	[[nodiscard]] std::vector<GenericDataTypeProxy> GetGenericDataTypes();
	[[nodiscard]] DataTypeProxy GetDataTypeProxyByName(std::string_view aName);

	template<IsPredicate<const DataTypeProxy&> FilterPredicate>
	[[nodiscard]] std::vector<DataTypeProxy> GetDataTypesFiltered(FilterPredicate&& aFilterPredicate);

	[[nodiscard]] std::vector<NodeTypeProxy> GetNodeTypes();

	[[nodiscard]] std::vector<FunctionProxy> GetFunctions();
	[[nodiscard]] std::vector<CustomEventProxy> GetCustomEvents();

	[[nodiscard]] std::vector<LinkProxy> GetTraversedLinks();

	using NodeTypeProxyIterator = ProxyGlobalIterator<NodeTypeID, NodeTypeProxy>;
	using NodeTypeProxyIteratorService = ProxyGlobalIteratorService<NodeTypeID, NodeTypeProxyIterator>;

	[[nodiscard]] NodeTypeProxyIteratorService IterateNodeTypes();
	[[nodiscard]] NodeTypeProxyIteratorService IterateNodeTypes(Predicate<NodeTypeProxy> aFilterPredicate);

	/*[[nodiscard]] std::vector<NodeTypeProxy> GetNodeTypesFilteredByRelatedDataTypesAndFlowTypeAndTrait(GenericDataTypeID aDataTypeID, eFlowType aFlowType, eNodeTrait aNodeTrait, bool(*aBitOperator)(eNodeTrait, eNodeTrait));
	[[nodiscard]] std::vector<NodeTypeProxy> GetNodeTypesFilteredByTrait(eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait) = HasFlag);*/

	[[nodiscard]] std::unordered_map<DataTypeProxy, std::vector<ClassProxy>> GetClasses();
	[[nodiscard]] std::vector<ClassProxy> GetClassesByTargetDataType(DataTypeProxy aDataTypeProxy);

	[[nodiscard]] inline DataTypeProxyIteratorService IterateDataTypes()
	{
		return DataTypeProxyIteratorService();
	}

	[[nodiscard]] inline DataTypeProxyIteratorService IterateDataTypes(Predicate<DataTypeProxy> aFilterPredicate)
	{
		return DataTypeProxyIteratorService(aFilterPredicate);
	}

	template<IsPredicate<const GenericDataTypeProxy&> FilterPredicate>
	[[nodiscard]] std::vector<GenericDataTypeProxy> GetGenericDataTypesFiltered(FilterPredicate&& aFilterPredicate)
	{
		const std::vector<GenericDataTypeProxy> dataTypes = GetGenericDataTypes();

		std::vector<GenericDataTypeProxy> filtered;
		filtered.reserve(dataTypes.size());

		for (const GenericDataTypeProxy& dataType : dataTypes)
		{
			if (aFilterPredicate(dataType))
			{
				filtered.push_back(dataType);
			}
		}

		return filtered;
	}
}
