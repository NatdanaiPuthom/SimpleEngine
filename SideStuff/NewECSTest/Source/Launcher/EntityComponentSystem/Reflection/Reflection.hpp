#pragma once
#include "ComponentRegistry.hpp"

namespace Simple
{
	class __RegisterComponentProperty final
	{
	public:
		template<typename DataType, typename Component>
		__RegisterComponentProperty(DataType Component::* aVariable, const char* aVariableName, const char* aCustomName = nullptr, const bool aShouldExpose = true, const bool aCanEdit = true)
		{
			ComponentRegistry::GetInstance()->RegisterProperty(aVariable, aVariableName, aCustomName, aShouldExpose, aCanEdit);
		}
	};
}

#define ECS_DATATYPE_NAME(aName) #aName
#define ECS_CONVERT_TO_STRING(aName) ECS_DATATYPE_NAME(aName)

#define ECS_COMBINE_STRINGS(aPropertyName, aNumberCounter) aPropertyName##aNumberCounter
#define ECS_UNIQUE_NAME(aPropertyName, aNumberCounter) ECS_COMBINE_STRINGS(aPropertyName, aNumberCounter)

#define REGISTER_COMPONENT_PROPERTY(aComponentName, aVariablePtr, ...) \
	inline Simple::__RegisterComponentProperty ECS_UNIQUE_NAME(ECS_Registered_Property_, ECS_COMBINE_STRINGS(aComponentName, __COUNTER__)) (aVariablePtr, Simple::ExtractVariableNameFromDataTypeName(ECS_CONVERT_TO_STRING(aVariablePtr)), __VA_ARGS__);