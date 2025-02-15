#pragma once
#include "ComponentRegistry.hpp"
#include "Macros.hpp"

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

	template<typename T>
	class __RegisterComponent final
	{
	public:
		__RegisterComponent()
		{
			ComponentRegistry::GetInstance()->RegisterComponentType<T>();
		}
	};
}

#define COMPONENT(ComponentType) \
    struct ComponentType; \
    inline static bool ComponentRegistered_##ComponentType = []() { \
       Simple::__RegisterComponent<ComponentType> Reflection_ECS_Registered_Component_##ComponentType; \
        return true; \
    }(); \
    struct ComponentType final

#define REGISTER_COMPONENT_PROPERTY(aComponentName, aVariablePtr, ...) \
    static inline Simple::__RegisterComponentProperty ECS_UNIQUE_NAME_2(ECS_Registered_Property_, __COUNTER__) (aVariablePtr, Simple::ExtractVariableNameFromDataTypeName(ECS_CONVERT_TO_STRING(aVariablePtr)), __VA_ARGS__);
