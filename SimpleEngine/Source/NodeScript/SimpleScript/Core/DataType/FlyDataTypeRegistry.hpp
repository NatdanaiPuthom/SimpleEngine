#pragma once
#include "../FlyDefines.hpp"
#include "FlyDataTypeManager.hpp"
#include "../Node/FlyNodeTypeRegistry.hpp"	
#include "../NodeTypes/OperatorNodes.hpp"

namespace FLY_NAMESPACE
{

	class DataTypeRegistry
	{
	public:

		template<typename T, eNodeOperatorTrait Operators = eNodeOperatorTrait::All, template<typename> typename... Templates>
		static void Register(const std::string& aName, const Color& aColor, bool aIsTargetable);

		template<template<typename> typename TemplateType>
		static void RegisterTemplateType(const std::string& aName);

		template<typename ClassType, typename MemberType>
		static void RegisterMemberVariable(MemberType ClassType::* aMemberVariable, const std::string& aName);

	private:

		template<typename T, template<typename> typename TemplateType>
		static void RegisterTemplateSpecification(const std::string& aTemplateArgumentName);

		template<typename T, template<typename> typename... TemplateTypes>
		static void RegisterTemplateTypes(const std::string& aTemplateArgumentName);

	};

	template<typename T, eNodeOperatorTrait Operators, template<typename> typename... Templates>
	inline void DataTypeRegistry::Register(const std::string& aName, const Color& aColor, const bool aIsTargetable)
	{
		DataTypeManager& dataTypeManager = Global::GetDataTypeManager();

		if (dataTypeManager.HasRegisteredType<T>())
		{
			return;
		}
		dataTypeManager.Register<T>(aName, aColor, aIsTargetable);


		if constexpr (DefaultConstructible<T>)
		{
			RegisterTemplateTypes<T, Templates...>(aName);
			RegisterGetterNodeType<T>();
			RegisterSetterNodeType<T>();
			RegisterOperatorNodeTypes<T, Operators>();
		}


		if constexpr (IsPointer<T>)
		{
			if (aIsTargetable)
			{
				NodeCreationData nodeData;
				nodeData.mName = aName + "/" + aName + "::Get Self";
				RegisterSystemNodeType<eNodeTrait::Self>(GetSelfNode<T>, std::move(nodeData));
			}
		}
	}

	template<template<typename> typename TemplateType>
	inline void DataTypeRegistry::RegisterTemplateType(const std::string& aName)
	{
		Global::GetDataTypeManager().RegisterTemplateType<TemplateType>(aName);
	}

	template<typename ClassType, typename MemberType>
	inline void DataTypeRegistry::RegisterMemberVariable(MemberType ClassType::* aMemberVariable, const std::string& aName)
	{
		DataTypeManager& dataTypeManager = Global::GetDataTypeManager();
		dataTypeManager.RegisterMemberVariable(aMemberVariable, aName);
		const std::string directory = GetClassNameFromMemberName(aName);
		NodeTypeRegistry::RegisterMemberVariable(aMemberVariable, directory, aName);
	}

	template<typename T, template<typename> typename TemplateType>
	inline void DataTypeRegistry::RegisterTemplateSpecification(const std::string& aTemplateName)
	{
		std::string templateTypeName;

		if (Global::GetDataTypeManager().mTemplateDataTypes.contains(typeid(TemplateType).hash_code()))
		{
			templateTypeName = Global::GetDataTypeManager().mTemplateDataTypes.at(typeid(TemplateType).hash_code()).name;
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

	struct PointerMember
	{
	};

	template<typename T>
	struct RegisterType
	{

		template<eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType<T> Struct(const char* aName, const Color& aColor = DefaultColor)
		{
			DataTypeRegistry::Register<T, Operators>(aName, aColor, false);

			return RegisterType<T>();
		}

		template<eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType<T> Class(const char* aName, const Color& aColor = DefaultColor)
		{
			const bool isTargetable = !ContainsType<NonTargetable, Traits...>;
			DataTypeRegistry::Register<T*, Operators>(aName, aColor, isTargetable);

			return RegisterType<T>();
		}
	};



	struct RegisterMemberVariable final
	{
		template<typename ParentType, typename MemberType, typename... Extra>
		constexpr RegisterMemberVariable(MemberType ParentType::* aMember, const std::string& aName, [[maybe_unused]] Extra&&... aExtra)
		{
			DataTypeRegistry::RegisterMemberVariable(aMember, aName);
		}
	};
}

#define FLY_CLASS(type, operators, color, ...) inline static FLY_NAMESPACE::RegisterType<type> fly_registeredType##type = FLY_NAMESPACE::RegisterType<type>::Class<operators, __VA_ARGS__>(#type, color);
#define FLY_STRUCT(type, operators, color, ...) inline static FLY_NAMESPACE::RegisterType<type> fly_registeredType##type = FLY_NAMESPACE::RegisterType<type>::Struct<operators, __VA_ARGS__>(#type, color);
#define FLY_MEMBER(member, ...) inline static FLY_NAMESPACE::RegisterMemberVariable prop(&member, #member);