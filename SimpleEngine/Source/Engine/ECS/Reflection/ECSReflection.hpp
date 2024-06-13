#pragma once
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/ECS/Reflection/ECSEditorFunctions.hpp"
#include "Engine/ECS/Reflection/ECSSerialisation.hpp"
#include "External/nlohmann/json.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <concepts>
#include <typeindex>

namespace ECS
{
	class __RegisterProperty;
	template<typename T> class __RegisterComponent;
	template<typename T> class __RegisterDataType;
}

namespace ECS
{
	template<typename T>
	concept Editable = requires(T & aData, const std::string & aVariableName)
	{
		{ ECS::ViewAndEditValue(aData, aVariableName) } -> std::same_as<bool>;
	};

	template<typename T>
	concept Saveable = requires(T & aData, const std::string & aVariableName)
	{
		{ ReturnDataAsJSON(aData, aVariableName) } -> std::same_as<nlohmann::json>;
	};

	struct ComponentProperty final
	{
		std::string name = "UnknownProperty";
		size_t id = 0;
		size_t byteOffset = 0;
		bool shouldExpose = true;
	};

	class TypeErasureComponent final
	{
	public:
		std::vector<ComponentProperty> myComponentProperties;
		std::string myComponentName;

		bool(*AddComponentFunctionPointer)(ECS::Entity aEntity) = nullptr;
		bool(*EditorFunctionPointer)(void* aData, const std::string& aVariableName) = nullptr;

		nlohmann::json(*GetDataAsJSON)(void* aData, const std::string& aVariableName) = nullptr;
	};

	class ComponentRegistry final
	{
		template<typename T> friend class ECS::__RegisterComponent;
		template<typename T>  friend class ECS::__RegisterDataType;
		friend class ECS::__RegisterProperty;
	public:
		static inline std::unordered_map<size_t, TypeErasureComponent> myTypeErasureComponents = {};
		static inline std::unordered_map<size_t, TypeErasureComponent> myTypeErasureDataTypes = {};
		static inline std::unordered_map<std::type_index, void(*)(void*)> myTypeErasureComponentDestructorInvoker;
	public:

		static inline void InspectComponentProperties(size_t aHashCode, void* aData, const std::string& aVariableName = "")
		{
			auto it = myTypeErasureComponents.find(aHashCode);

			if (it != myTypeErasureComponents.end())
			{
				if (it->second.EditorFunctionPointer != nullptr)
				{
					it->second.EditorFunctionPointer(aData, aVariableName);
				}

				for (auto& componentProperty : it->second.myComponentProperties)
				{
					if (componentProperty.shouldExpose == false)
					{
						continue;
					}

					ExposeProperty(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.name);
				}
			}
		}

	private:
		template<typename T>
		static inline void RegisterComponentType()
		{
			const bool alreadyExistOrHashCollision = myTypeErasureComponents.contains(typeid(T).hash_code());

			if (alreadyExistOrHashCollision == true)
			{
				assert(false && "Component already exist or has hash collision!");
				return;
			}

			TypeErasureComponent typeErasureComponent;

			typeErasureComponent.myComponentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

			typeErasureComponent.AddComponentFunctionPointer = [](ECS::Entity aEntity) -> bool
				{
					return aEntity->AddComponent<T>();
				};

			if constexpr (ECS::Editable<T>)
			{
				typeErasureComponent.EditorFunctionPointer = [](void* aDataPointer, const std::string& aVariableName) -> bool
					{
						T* pointer = reinterpret_cast<T*>(aDataPointer);
						return ECS::ViewAndEditValue(*pointer, aVariableName + "##" + std::to_string(reinterpret_cast<size_t>(aDataPointer)));
					};
			}

			myTypeErasureComponents[typeid(T).hash_code()] = typeErasureComponent;

			myTypeErasureComponentDestructorInvoker[typeid(T)] = [](void* aPointer) -> void
				{
					static_cast<T*>(aPointer)->~T();
				};
		}

		template<typename T>
		static inline void RegisterDataType()
		{
			const bool alreadyExistOrHashCollision = myTypeErasureDataTypes.contains(typeid(T).hash_code());

			if (alreadyExistOrHashCollision == true)
			{
				assert(false && "Component already exist or has hash collision!");
				return;
			}

			TypeErasureComponent dataType;

			dataType.myComponentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

			dataType.AddComponentFunctionPointer = [](ECS::Entity aEntity) -> bool
				{
					return aEntity->AddComponent<T>();
				};

			if constexpr (ECS::Editable<T>)
			{
				dataType.EditorFunctionPointer = [](void* aDataPointer, const std::string& aVariableName) -> bool
					{
						T* pointer = reinterpret_cast<T*>(aDataPointer);
						return ECS::ViewAndEditValue(*pointer, aVariableName + "##" + std::to_string(reinterpret_cast<size_t>(aDataPointer)));
					};
			}

			if constexpr (ECS::Saveable<T>)
			{
				dataType.GetDataAsJSON = [](void* aDataPointer, const std::string& aVariableName) -> nlohmann::json
					{
						T* pointer = reinterpret_cast<T*>(aDataPointer);
						return ReturnDataAsJSON(*pointer, aVariableName);
					};
			}

			myTypeErasureDataTypes[typeid(T).hash_code()] = dataType;
		}

		static inline void ExposeProperty(size_t aHashCode, void* aData, const std::string& aVariableName = "")
		{
			auto it = myTypeErasureDataTypes.find(aHashCode);

			if (it != myTypeErasureDataTypes.end())
			{
				if (it->second.EditorFunctionPointer != nullptr)
				{
					it->second.EditorFunctionPointer(aData, aVariableName);
				}

				for (auto& componentProperty : it->second.myComponentProperties)
				{
					ExposeProperty(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.name);
				}
			}
		}

		template<typename DataType, typename Component>
		static inline void RegisterProperty(DataType Component::* aVariable, const std::string& aVariableName, const bool aShouldExpose)
		{
			ComponentProperty componentProperty;
			componentProperty.name = aVariableName;
			componentProperty.id = typeid(DataType).hash_code();
			componentProperty.byteOffset = GetByteOffset(aVariable);
			componentProperty.shouldExpose = aShouldExpose;

			const bool componentDoesExist = myTypeErasureComponents.contains(typeid(Component).hash_code());

			if (componentDoesExist == false)
			{
				assert(false && "Component type does not exist in register. Please register the component first before it's properties");
				return;
			}

			myTypeErasureComponents[typeid(Component).hash_code()].myComponentProperties.push_back(componentProperty);
		}

		template<typename DataType, typename PropertyType>
		static inline constexpr size_t GetByteOffset(PropertyType DataType::* aProperty)
		{
			constexpr DataType* nullDataType = nullptr;
			return (size_t) & reinterpret_cast<const char&>(nullDataType->*aProperty); //NOTE(v11.0.1): this is supposed to be null?
		}
	};

	template<typename T>
	class __RegisterComponent final
	{
	public:
		__RegisterComponent()
		{
			ECS::ComponentRegistry::RegisterComponentType<T>();
		}
	};

	template<typename T>
	class __RegisterDataType final
	{
	public:
		__RegisterDataType()
		{
			ECS::ComponentRegistry::RegisterDataType<T>();
		}
	};

	class __RegisterProperty final
	{
	public:
		template<typename DataType, typename Component>
		__RegisterProperty(DataType Component::* aVariable, const char* aVariableName, const bool aShouldExpose = true)
		{
			ECS::ComponentRegistry::RegisterProperty(aVariable, aVariableName, aShouldExpose);
		}
	};
}

#include "Engine/ECS/Reflection/ECSMacros.hpp"

#define REGISTER_DATATYPE(aDataType) inline ECS::__RegisterDataType<aDataType> Global_ECS_Registered_Datatype_##aDataType;
#define REGISTER_COMPONENT(aComponent) inline ECS::__RegisterComponent<aComponent> Global_ECS_Registered_Component_##aComponent;
#define REGISTER_AND_EXPOSE_PROPERTY(aVariable, ...) inline ECS::__RegisterProperty COMBINE_FOR_UNIQUE_NAME(Global_ECS_Registered_Property_, __COUNTER__) = ECS::__RegisterProperty(aVariable, ECS::ExtractVariableNameFromDataTypeName(CONVERT_TO_STRING(aVariable)), __VA_ARGS__); //NOTE(v11.0.0): where does __COUNTER__ macro came from?. But it works.

//TO-DO(v11.0.3): maybe figure out a more modular way to register different type of const, pointers and array of different sizes
#define REGISTER_DATATYPE_CONST_POINTER(aDataType) inline ECS::__RegisterDataType<const aDataType*> Global_ECS_Registered_Datatype_Const_Pointer_##aDataType;
#define REGISTER_DATATYPE_ARRAY_CONST_POINTER(aArray, aDataType, aSize) inline ECS::__RegisterDataType<aArray<const aDataType*, aSize>> Global_ECS_Registered_Datatype_Array_Const_Pointer_##aArray;