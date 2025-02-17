#pragma once
#include "TypeErasureStructs.hpp"
#include "../Utilities/StringFormatting.hpp"
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

namespace Simple
{
	class ComponentRegistry final
	{
	public:
		std::unordered_map<size_t, TypeErasureComponentObject> myTypeErasureComponents;
		std::unordered_map<std::string, size_t> myComponentNameToHashCode;
	public:

		static ComponentRegistry* GetInstance()
		{
			if (myInstance == nullptr)
			{
				myInstance = new ComponentRegistry(); //NOTE: We need to create a new instance from GetInstance because we cannot guarantee the order of Initialization of inline variables
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

			if (myTypeErasureComponents.contains(hashCode) && myTypeErasureComponents.at(hashCode).isAdded == true)
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

			if (myTypeErasureComponents.contains(hashCode) == false && myTypeErasureComponents.at(hashCode).isAdded == false)
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

	private:
		static inline ComponentRegistry* myInstance = nullptr;
	};
}