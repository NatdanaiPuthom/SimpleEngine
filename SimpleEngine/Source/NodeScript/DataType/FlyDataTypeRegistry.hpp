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


		if constexpr (PointerType<T>)
		{
			if (aIsTargetable)
			{
				NodeCreationData nodeData;
				nodeData.mName = aName + "/" + aName + "::Get Target";
				RegisterSystemNodeType<eNodeTrait::Target>(GetTargetNode<T>, std::move(nodeData));
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

	struct RegisterType final
	{

		template<typename T, eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType Struct_Impl(const char* aName, [[maybe_unused]] Traits&&... aTraits)
		{
			Color color = DefaultColor;
			if constexpr (ContainsType<Color, Traits...>)
			{
				color = Extract<Color>(std::forward<Traits>(aTraits)...);
			}
			DataTypeRegistry::Register<T, Operators>(aName, color, false);

			return RegisterType{};
		}

		template<typename T, typename... Traits>
		constexpr static RegisterType Struct(const char* aName, Traits&&... aTraits)
		{
			return Struct_Impl<T, eNodeOperatorTrait::All, Traits...>(aName, std::forward<Traits>(aTraits)...);
		}

		template<typename T, eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType Struct(const char* aName, Traits&&... aTraits)
		{
			return Struct_Impl<T, Operators, Traits...>(aName, std::forward<Traits>(aTraits)...);
		}

		template<typename T, eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType Class_Impl(const char* aName, [[maybe_unused]] Traits&&... aTraits)
		{
			const bool isTargetable = !ContainsType<NonTargetable, Traits...>;
			Color color = DefaultColor;
			if constexpr (ContainsType<Color, Traits...>)
			{
				color = Extract<Color>(std::forward<Traits>(aTraits)...);
			}
			DataTypeRegistry::Register<T*, Operators>(aName, color, isTargetable);

			return RegisterType{};
		}

		template<typename T, typename... Traits>
		constexpr static RegisterType Class(const char* aName, Traits&&... aTraits)
		{
			return Class_Impl<T, eNodeOperatorTrait::None, Traits...>(aName, std::forward<Traits>(aTraits)...);
		}

		template<typename T, eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType Class(const char* aName, Traits&&... aTraits)
		{
			return Class_Impl<T, Operators, Traits...>(aName, std::forward<Traits>(aTraits)...);
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

#define FLY_CLASS(Type, ...) inline static FLY_NAMESPACE::RegisterType fly_registeredType##Type = FLY_NAMESPACE::RegisterType::Class<Type>(#Type, __VA_ARGS__);
#define FLY_STRUCT(Type, ...) inline static FLY_NAMESPACE::RegisterType fly_registeredType##Type = FLY_NAMESPACE::RegisterType::Struct<Type>(#Type, __VA_ARGS__);

												//inline static Fly::RegisterFunctionNode FLY_UNIQUE_NAME(fly_function) = FLY_NAMESPACE::RegisterFunctionNode::Register(&function, #function, __VA_ARGS__);

#define FLY_MEMBER(member, ...) inline static FLY_NAMESPACE::RegisterMemberVariable prop(&member, #member);