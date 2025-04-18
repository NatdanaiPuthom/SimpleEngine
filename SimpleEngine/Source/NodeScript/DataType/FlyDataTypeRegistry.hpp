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
		static void Register(const TypeParameters& aTypeParameters);

		template<typename ClassType, typename MemberType>
		static void RegisterMemberVariable(MemberType ClassType::* aMemberVariable, const std::string& aName);

	};

	template<typename T, eNodeOperatorTrait Operators, bool Container>
	inline void DataTypeRegistry::Register(const TypeParameters& aTypeParameters)
	{
		DataTypeManager& dataTypeManager = Internal::GetDataTypeManager();

		if (dataTypeManager.IsRegistered<T>())
		{
			return;
		}
		dataTypeManager.Register<T>(aTypeParameters);


		if constexpr (DefaultConstructible<T>)
		{
			RegisterGetterNodeType<T>();
			RegisterSetterNodeType<T>();
			RegisterOperatorNodeTypes<T, Operators>();
		}


		if constexpr (PointerType<T>)
		{
			if (aTypeParameters.mIsTargetable)
			{
				NodeCreationData nodeData;
				nodeData.mName = aTypeParameters.mName + "/" + aTypeParameters.mName + "::Get Target";
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
					const std::string& typeName = Internal::GetDataTypeManager().Find<T>()->Name();

					TypeParameters containerTypeParams;
					containerTypeParams.mName = "Vector <" + typeName + ">";
					containerTypeParams.mColor = Colors::Purple;
					containerTypeParams.mIsTargetable = false;
					containerTypeParams.mRegisterPointer = true;

					Register<VectorType, eNodeOperatorTrait::None, true>(containerTypeParams);


					RegisterSystemNodeType(ForEachNode<VectorType>, NodeCreationData{ .mName = "Execution/For Each " + typeName });

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

	struct NonTargetable final
	{
	};

	struct CustomName final
	{
		std::string mName;
	};

	struct RegisterType final
	{

		template<typename T, eNodeOperatorTrait Operators, typename... Traits>
		constexpr static RegisterType ValueType_Impl(const std::string& aName, [[maybe_unused]] Traits&&... aTraits)
		{
			const Color color = TryExtract(Internal::GetDataTypeManager().GetDefaultColor(), std::forward<Traits>(aTraits)...);
			const CustomName customName = TryExtract(CustomName{ aName }, std::forward<Traits>(aTraits)...);

			TypeParameters typeParameters;
			typeParameters.mName = customName.mName;
			typeParameters.mColor = color;
			typeParameters.mIsTargetable = false;
			typeParameters.mRegisterPointer = true;

			DataTypeRegistry::Register<T, Operators>(typeParameters);

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

			const Color color = TryExtract(Internal::GetDataTypeManager().GetDefaultColor(), std::forward<Traits>(aTraits)...);
			const CustomName customName = TryExtract(CustomName{ aName }, std::forward<Traits>(aTraits)...);

			TypeParameters typeParameters;
			typeParameters.mName = customName.mName;
			typeParameters.mColor = color;
			typeParameters.mIsTargetable = isTargetable;
			typeParameters.mRegisterPointer = false;

			DataTypeRegistry::Register<T*, Operators>(typeParameters);

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

#define FLY_POINTERTYPE(Type, ...) inline FLY_NAMESPACE::RegisterType FLY_UNIQUE_NAME(fly_pointertype) = FLY_NAMESPACE::RegisterType::PointerType<Type>(#Type, __VA_ARGS__);
#define FLY_VALUETYPE(Type, ...) inline FLY_NAMESPACE::RegisterType FLY_UNIQUE_NAME(fly_valuetype) = FLY_NAMESPACE::RegisterType::ValueType<Type>(#Type, __VA_ARGS__);

#define FLY_MEMBER(member) inline static FLY_NAMESPACE::RegisterMemberVariable FLY_UNIQUE_NAME(fly_member) = FLY_NAMESPACE::RegisterMemberVariable::Member(&member, #member);