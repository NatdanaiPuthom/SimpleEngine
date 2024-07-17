#pragma once
#include "../ScriptDefines.h"
#include <iostream>


template<typename T>
concept Editable3 = requires(T & aValue)
{
	{ Edit(aValue) } -> std::same_as<bool>;
};

//template<typename T>
//concept Editable4 = requires(T & aValue)
//{
//	{ ::Edit(aValue) } -> std::same_as<bool>;
//};

namespace SCR
{
	template <typename... Types>
	concept EmptyParameterPack = sizeof...(Types) == 0;

	template<typename T>
	concept CleanType = std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, T>;

	template<typename T>
	struct CleanUpType
	{
		using type = std::remove_const_t<std::remove_reference_t<T>>;
	};

	template<typename T>
	using CleanType_V = CleanUpType<T>::type;

	template<typename T1, typename T2>
	concept IsSameType = std::is_same_v<T1, T2>;

	template<typename T>
	concept IsPointer = std::is_pointer_v<T>;

	template<typename T>
	concept IsNotPointer = !IsPointer<T>;

	template<typename T>
	concept IsVoid = std::is_void_v<T>;

	template<typename T>
	concept IsNotVoid = !IsVoid<T>;

	template<typename T>
	concept CleanType_NonPointer = requires
	{
		CleanType<T> && !IsPointer<T>;
	};

	template<int Value, int Min, int Max>
	concept IsInRange = Value >= Min && Value <= Max;

	template <typename T>
	struct Is_Of_Template_Type : std::false_type {};

	template <template <typename...> typename Base, typename... Ts>
	struct Is_Of_Template_Type<Base<Ts...>> : std::true_type {};

	template <template <typename...> typename Base, typename T>
	concept IsSameTemplateType = Is_Of_Template_Type<T>::value;

	template <typename T, size_t Size>
	concept SizeEqual = (sizeof(T) == Size);

	template <typename T, size_t Size>
	concept SizeNotEqual = (sizeof(T) != Size);

	template <typename T, size_t Size>
	concept SizeGreaterEqual = (sizeof(T) >= Size);

	template <typename T, size_t Size>
	concept SizeLessEqual = (sizeof(T) <= Size);

	template <typename T, size_t Size>
	concept SizeGreater = (sizeof(T) > Size);

	template <typename T, size_t Size>
	concept SizeLess = (sizeof(T) < Size);


	template<typename T>
	concept IsSmallObject = SizeLessEqual<T, 64>;

	// Operator concepts
	template<typename T>
	concept HasOperator_EqualTo = requires(T a, T b)
	{
		{ a == b } -> std::same_as<bool>;
	};

	template<typename T>
	concept HasOperator_NotEqualTo = requires(T a, T b)
	{
		{ a != b } -> std::same_as<bool>;
	};

	template<typename T>
	concept HasOperator_Greater = requires(T a, T b)
	{
		{ a > b } -> std::same_as<bool>;
	};

	template<typename T>
	concept HasOperator_Less = requires(T a, T b)
	{
		{ a < b } -> std::same_as<bool>;
	};

	template<typename T>
	concept HasOperator_GreaterEqual = requires(T a, T b)
	{
		{ a >= b } -> std::same_as<bool>;
	};

	template<typename T>
	concept HasOperator_LessEqual = requires(T a, T b)
	{
		{ a <= b } -> std::same_as<bool>;
	};

	template<typename T>
	concept HasOperator_And = requires(T a, T b)
	{
		{ a&& b } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_Or = requires(T a, T b)
	{
		{ a || b } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_Not = requires(T a)
	{
		{ !a } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_Print = requires(T a)
	{
		{ std::cout << a };
	};

	template<typename T>
	concept HasOperator_Add = requires(T a, T b)
	{
		{ a + b } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_Subtract = requires(T a, T b)
	{
		{ a - b } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_Multiply = requires(T a, T b)
	{
		{ a* b } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_Divide = requires(T a, T b)
	{
		{ a / b } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_Modulo = requires(T a, T b)
	{
		{ a% b } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_IncrementPrefix = requires(T a)
	{
		{ ++a } -> std::same_as<T&>;
	};

	template<typename T>
	concept HasOperator_IncrementPostfix = requires(T a)
	{
		{ a++ } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_DecrementPrefix = requires(T a)
	{
		{ --a } -> std::same_as<T&>;
	};

	template<typename T>
	concept HasOperator_DecrementPostfix = requires(T a)
	{
		{ a-- } -> std::same_as<T>;
	};

	template<typename T>
	concept HasOperator_Functor = requires(T a)
	{
		{ a() } -> std::same_as<void>;
	};

	template<typename CastFrom, typename CastTo>
	concept IsStaticCastable = requires (CastFrom a)
	{
		{ static_cast<CastTo>(a) } -> std::same_as<CastTo>;
	};

	template<typename Callable, typename ReturnType, typename... Args>
	concept ReturnsType = requires (Callable aCallable, Args&&... args)
	{
		{ aCallable(std::forward<Args>(args)...) } -> std::same_as<ReturnType>;
	};

	template<typename T, typename... Args>
	concept Predicate = ReturnsType<T, bool, Args...>;

	template<typename T, typename Base>
	concept IsBaseOf = std::is_base_of_v<Base, T>;

	template<typename T, typename... Args>
	concept HasArgsConstructor = requires(Args&&... args)
	{
		T(std::forward<Args>(args)...);
	};

	template<typename T, typename... Inputs>
	concept ValidCallableInputs = requires(T a, Inputs&&... inputs)
	{
		a(std::forward<Inputs>(inputs)...);
	};

	template<typename T, typename Output>
	concept ValidCallableOutput = requires(T a)
	{
		{ a() } -> std::same_as<Output>;
	};

	template<typename... Args>
	struct HasReference
	{
		static constexpr bool value = (std::is_reference_v<Args> || ...);
	};

	template<>
	struct HasReference<>
	{
		static constexpr bool value = false;
	};

	template<typename... Args>
	concept NoArgsReference = !HasReference<Args...>::value;

	template<typename T, typename... Types>
	concept ContainsType = (std::same_as<T, Types> || ...);

	template<typename Find, size_t Index>
	constexpr size_t GetIndexOfTypeFromArgsImpl()
	{
		return InvalidID<size_t>();
	}

	template<typename Find, size_t Index, typename Current, typename... Rest>
	constexpr size_t GetIndexOfTypeFromArgsImpl()
	{
		if constexpr (std::same_as<Find, Current>)
		{
			return Index;
		}
		else
		{
			return GetIndexOfTypeFromArgsImpl<Find, Index + 1, Rest...>();
		}
	}

	template<typename Find, typename... Args>
	constexpr size_t GetIndexOfTypeFromArgs()
	{
		return GetIndexOfTypeFromArgsImpl<Find, 0, Args...>();
	}

	template<typename T>
	concept Editable = requires(T & aValue)
	{
		{ Edit(aValue) } -> std::same_as<bool>;
	};

	/*template<typename T>
	concept Editable2 = requires(T & aValue)
	{
		{ ::Edit(aValue) } -> std::same_as<bool>;
	};*/

	template<typename T, typename SerializationObject>
	concept Savable = requires(const T & aValue, SerializationObject & aSerializationObject)
	{
		{ Save(aSerializationObject, aValue) } -> std::same_as<void>;
	};

	template<typename T, typename SerializationObject>
	concept Loadable = requires(T & aValue, const SerializationObject & aSerializationObject)
	{
		{ Load(aSerializationObject, aValue) } -> std::same_as<void>;
	};

	template<typename T>
	concept DefaultConstructible = std::default_initializable<T>;

	template<typename T>
	concept Copyable = std::copyable<T>;

	template<typename T, typename SerializationObject>
	concept Scriptable =
		CleanType<T> &&
		Editable<T> &&
		Savable<T, SerializationObject>&&
		Loadable<T, SerializationObject>&&
		DefaultConstructible<T>&&
		Copyable<T>;

	template<typename Type, typename CurrentType, typename... Types>
	constexpr bool PackContains()
	{
		if constexpr (IsSameType<Type, CurrentType>)
		{
			return true;
		}
		else
		{
			if constexpr (EmptyParameterPack<Types...>)
			{
				return false;
			}
			else
			{
				return PackContains<Type, Types...>();
			}
		}
	}

	template<typename ClassType, typename PropertyType>
	constexpr size_t GetByteOffset(PropertyType ClassType::* aProperty)
	{
		constexpr ClassType* a = nullptr;
		return (size_t) & reinterpret_cast<const char&>(a->*aProperty);
	}
}