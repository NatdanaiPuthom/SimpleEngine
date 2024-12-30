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

		template<typename T, eNodeOperatorTrait Operators = eNodeOperatorTrait::All, bool Container = false>
		static void Register(const std::string& aName, const Color& aColor, bool aIsTargetable);

		template<typename ClassType, typename MemberType>
		static void RegisterMemberVariable(MemberType ClassType::* aMemberVariable, const std::string& aName);

	};

	template<typename T, eNodeOperatorTrait Operators, bool Container>
	inline void DataTypeRegistry::Register(const std::string& aName, const Color& aColor, const bool aIsTargetable)
	{
		DataTypeManager& dataTypeManager = Internal::GetDataTypeManager();

		if (dataTypeManager.IsRegistered<T>())
		{
			return;
		}
		dataTypeManager.Register<T>(aName, aColor, aIsTargetable);


		if constexpr (DefaultConstructible<T>)
		{
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

		if constexpr (!Container)
		{

			if constexpr (TypeDefined<FlyCustomVectorType>)
			{
				using VectorType = FlyCustomVectorType::template type<T>;

				VectorType v{};
				v;

				if constexpr (!std::same_as<VectorType, std::vector<bool>>)
				{
					const std::string& typeName = Internal::GetDataTypeManager().GetName(Fly::GetDataTypeID<T>());
					Register<VectorType, eNodeOperatorTrait::None, true>("Vector <" + typeName + ">", Colors::Purple, false);


					RegisterSystemNodeType(ForEach<VectorType>, NodeCreationData{ .mName = "Execution/For Each " + typeName });

				}

			}
			else
			{
				using VectorType = std::vector<T>;

				VectorType v{};
				v;
			}
		}

	}

	template<typename ClassType, typename MemberType>
	inline void DataTypeRegistry::RegisterMemberVariable(MemberType ClassType::* aMemberVariable, const std::string& aName)
	{
		DataTypeManager& dataTypeManager = Internal::GetDataTypeManager();
		dataTypeManager.RegisterMemberVariable(aMemberVariable, aName);
		const std::string directory = GetClassNameFromMemberName(aName);
		NodeTypeRegistry::RegisterMemberVariable(aMemberVariable, directory, aName);
	}

	struct NonTargetable
	{
	};

	struct RegisterType final
	{

		template<typename T, eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType ValueType_Impl(const char* aName, [[maybe_unused]] Traits&&... aTraits)
		{
			Color color = Internal::GetDataTypeManager().GetDefaultColor();
			if constexpr (ContainsType<Color, Traits...>)
			{
				color = Extract<Color>(std::forward<Traits>(aTraits)...);
			}
			DataTypeRegistry::Register<T, Operators>(aName, color, false);

			return RegisterType{};
		}

		template<typename T, typename... Traits>
		constexpr static RegisterType ValueType(const char* aName, Traits&&... aTraits)
		{
			return ValueType_Impl<T, eNodeOperatorTrait::All, Traits...>(aName, std::forward<Traits>(aTraits)...);
		}

		template<typename T, eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType ValueType(const char* aName, Traits&&... aTraits)
		{
			return ValueType_Impl<T, Operators, Traits...>(aName, std::forward<Traits>(aTraits)...);
		}

		template<typename T, eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType PointerType_Impl(const char* aName, [[maybe_unused]] Traits&&... aTraits)
		{
			const bool isTargetable = !ContainsType<NonTargetable, Traits...>;
			Color color = Internal::GetDataTypeManager().GetDefaultColor();
			if constexpr (ContainsType<Color, Traits...>)
			{
				color = Extract<Color>(std::forward<Traits>(aTraits)...);
			}
			DataTypeRegistry::Register<T*, Operators>(aName, color, isTargetable);

			return RegisterType{};
		}

		template<typename T, typename... Traits>
		constexpr static RegisterType PointerType(const char* aName, Traits&&... aTraits)
		{
			return PointerType_Impl<T, eNodeOperatorTrait::None, Traits...>(aName, std::forward<Traits>(aTraits)...);
		}

		template<typename T, eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType PointerType(const char* aName, Traits&&... aTraits)
		{
			return PointerType_Impl<T, Operators, Traits...>(aName, std::forward<Traits>(aTraits)...);
		}
	};



	struct RegisterMemberVariable final
	{
		constexpr RegisterMemberVariable()
		{
		}

		template<typename ParentType, typename MemberType, typename... Extra>
		static constexpr RegisterMemberVariable Member(MemberType ParentType::* aMember, const std::string& aName, [[maybe_unused]] Extra&&... aExtra)
		{
			DataTypeRegistry::RegisterMemberVariable(aMember, aName);

			return RegisterMemberVariable();
		}
	};
}

#define FLY_POINTERTYPE(Type, ...) inline static FLY_NAMESPACE::RegisterType fly_registeredType##Type = FLY_NAMESPACE::RegisterType::PointerType<Type>(#Type, __VA_ARGS__);
#define FLY_VALUETYPE(Type, ...) inline static FLY_NAMESPACE::RegisterType fly_registeredType##Type = FLY_NAMESPACE::RegisterType::ValueType<Type>(#Type, __VA_ARGS__);

#define FLY_MEMBER(member, ...) inline static FLY_NAMESPACE::RegisterMemberVariable FLY_UNIQUE_NAME(fly_member) = FLY_NAMESPACE::RegisterMemberVariable::Member(&member, #member);