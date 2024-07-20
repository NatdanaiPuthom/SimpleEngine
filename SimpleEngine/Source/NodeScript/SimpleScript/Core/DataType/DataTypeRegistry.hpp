#pragma once
#include "../ScriptDefines.hpp"
#include "DataTypeManager.hpp"
#include "../Node/NodeTypeRegistry.hpp"	
#include "../NodeTypes/OperatorNodes.hpp"

namespace FLY_NAMESPACE
{

	class DataTypeRegistry
	{
	public:

		template<typename T, eNodeOperatorTrait Operators = eNodeOperatorTrait::All, template<typename> typename... Templates>
		static void Register(const std::string & aName, const Color& aColor, bool aIsTargetable);

		template<template<typename> typename TemplateType>
		static void RegisterTemplateType(const std::string& aName);

		template<typename ClassType, typename PropertyType>
		static void RegisterProperty(PropertyType ClassType::* aProperty, const std::string& aName);

	private:

		template<typename T, template<typename> typename TemplateType>
		static void RegisterTemplateSpecification(const std::string& aTemplateArgumentName);

		template<typename T, template<typename> typename... TemplateTypes>
		static void RegisterTemplateTypes(const std::string& aTemplateArgumentName);

	};

	template<typename T, eNodeOperatorTrait Operators, template<typename> typename... Templates>
	inline void DataTypeRegistry::Register(const std::string & aName, const Color & aColor, const bool aIsTargetable)
	{
		DataTypeManager& dataTypeManager = Global::GetDataTypeManager();

		if (dataTypeManager.HasRegisteredType<T>())
		{
			return;
		}
		dataTypeManager.Register<T>(aName, aColor, aIsTargetable);

		RegisterTemplateTypes<T, Templates...>(aName);
		RegisterGetterNodeType<T>();
		RegisterSetterNodeType<T>();
		RegisterOperatorNodeTypes<T, Operators>();

		if (aIsTargetable)
		{
			NodeTypeRegistry::RegisterNodeType(GetSelfNode<T>, aName + "/" + aName + "::Get Self");

		}
	}

	template<template<typename> typename TemplateType>
	inline void DataTypeRegistry::RegisterTemplateType(const std::string& aName)
	{
		Global::GetDataTypeManager().RegisterTemplateType<TemplateType>(aName);
	}

	template<typename ClassType, typename PropertyType>
	inline void DataTypeRegistry::RegisterProperty(PropertyType ClassType::* aProperty, const std::string& aName)
	{
		Global::GetDataTypeManager().RegisterProperty(aProperty, aName);
		NodeTypeRegistry::RegisterMemberVariable(aProperty, "SFSF", "");
	}

	template<typename T, template<typename> typename TemplateType>
	inline void DataTypeRegistry::RegisterTemplateSpecification(const std::string& aTemplateName)
	{
		std::string templateTypeName;

		if (Global::GetDataTypeManager().myTemplateDataTypes.contains(typeid(TemplateType).hash_code()))
		{
			templateTypeName = Global::GetDataTypeManager().myTemplateDataTypes.at(typeid(TemplateType).hash_code()).name;
		}
		else
		{
			templateTypeName = typeid(TemplateType).name();
		}

		using Specification = TemplateType<T>;

		Global::GetDataTypeManager().RegisterTemplateSpecification<Specification>(templateTypeName + std::string("<") + aTemplateName + ">", DefaultColor);
		RegisterGetterNodeType<Specification>();
		RegisterSetterNodeType<Specification>();
		RegisterOperatorNodeTypes<Specification, eNodeOperatorTrait::All>();
	}

	template<typename T, template<typename> typename... TemplateTypes>
	inline void DataTypeRegistry::RegisterTemplateTypes(const std::string& aTemplateName)
	{
		(RegisterTemplateSpecification<T, TemplateTypes>(aTemplateName), ...);
	}

	enum class eDataTypeSetting
	{
		Serializable,

	};

	struct NonTargetable
	{
	};

	template<typename T>
	struct RegisterType
	{

		template<eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType<T> Register(const char* aName, const Color& aColor = DefaultColor)
		{
			constexpr bool isNotTargetable = ContainsType<NonTargetable, Traits...>;
			DataTypeRegistry::Register<T, Operators>(aName, aColor, !isNotTargetable);

			return RegisterType<T>();
		}
	};



	struct RegisterProperty
	{
		template<typename StructType, typename MemberType>
		constexpr RegisterProperty(MemberType StructType::* aMember, const std::string& aName)
		{
			DataTypeRegistry::RegisterProperty(aMember, aName);
			//SCRIPT::NodeTypeRegistry::RegisterMemberVariable(aMember, );
		}
	};
}

#define FLY_DATATYPE(type, operators, color, ...) inline static FLY_NAMESPACE::RegisterType<type> fly_registeredType##type = FLY_NAMESPACE::RegisterType<type>::Register<operators, __VA_ARGS__>(#type, color);
#define FLY_PROPERTY(member) inline static FLY_NAMESPACE::RegisterProperty prop(member, #member);