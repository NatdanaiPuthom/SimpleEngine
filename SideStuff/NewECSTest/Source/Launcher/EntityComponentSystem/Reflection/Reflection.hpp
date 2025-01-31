#pragma once
#include "../Utilities/StringFormatting.hpp"
#include <string>
#include <limits>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

namespace Simple
{
	struct ComponentProperty final
	{
		std::string name = "Unknown Property Name";
		std::string customVariableName = "Unknown Custom Property Name";

		size_t id = Invalid;
		size_t byteOffset = 0;

		bool isVisible = true;
		bool isEditable = true;

		static constexpr size_t Invalid = std::numeric_limits<size_t>::max();
	};

	struct TypeErasureComponentObject final
	{
		std::string name;
		std::string formattedName;
		std::vector<ComponentProperty> properties;
		size_t size = 0;
		bool isAdded = false;
	};

	class ComponentRegistry final
	{
	public:
		static ComponentRegistry* GetInstance()
		{
			if (myInstance == nullptr)
			{
				myInstance = new ComponentRegistry();
			}

			return myInstance;
		}

		void DestroyInstance()
		{
			if (myInstance != nullptr)
			{
				delete myInstance;
				myInstance = nullptr;
			}
		}

		template<typename T>
		void RegisterComponentType()
		{
			const size_t hashCode = typeid(T).hash_code();

			if (myTypeErasureComponents.contains(hashCode))
			{
				throw std::runtime_error("Component with hash code " + std::to_string(hashCode) + " already exists");
			}

			TypeErasureComponentObject typeErasureComponent;
			typeErasureComponent.name = ConvertTypeIndexNameToPrettyName(typeid(T).name());
			typeErasureComponent.formattedName = RemoveSubStringIfExist(typeErasureComponent.name, "Component");
			typeErasureComponent.size = sizeof(T);
			typeErasureComponent.isAdded = true;

			myTypeErasureComponents[hashCode] = typeErasureComponent;
			myComponentNameToHashCode[typeErasureComponent.name] = hashCode;
		}

		template<typename DataType, typename Component>
		void RegisterProperty(DataType Component::* aVariablePtr, const std::string& aVariableName, const char* aCustomName, const bool aShouldExpose, const bool aCanEdit)
		{
			const size_t hashCode = typeid(Component).hash_code();

			if (myTypeErasureComponents.contains(hashCode) == false)
			{
				RegisterComponentType<Component>();
			}

			ComponentProperty componentProperty;
			componentProperty.name = aVariableName;
			componentProperty.customVariableName = aCustomName ? aCustomName : ConvertAndAddSpaceToSubStringWithUpperCase(aVariableName);
			componentProperty.id = typeid(DataType).hash_code();
			componentProperty.byteOffset = GetByteOffset(aVariablePtr);
			componentProperty.isVisible = aShouldExpose;
			componentProperty.isEditable = aCanEdit;

			myTypeErasureComponents[hashCode].properties.push_back(componentProperty);
		}


		template<typename DataType, typename PropertyType>
		inline constexpr size_t GetByteOffset(PropertyType DataType::* aProperty)
		{
			return ((::size_t) & reinterpret_cast<char const volatile&>((((DataType*)0)->*aProperty)));
		}

	public:
		std::unordered_map<size_t, TypeErasureComponentObject> myTypeErasureComponents;
		std::unordered_map<std::string, size_t> myComponentNameToHashCode;

		static inline ComponentRegistry* myInstance = nullptr;
	};

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