#pragma once
#include "../FlyDefines.hpp"
#include <iostream>
#include <variant>

namespace FLY_NAMESPACE
{

	template<typename...>
	struct TypeList { };

	template<auto>
	struct Info
	{
		constexpr auto friend get(Info);
	};

	template<typename T>
	struct Type
	{
		using value_type = T;
		static void id() {}

		constexpr auto friend get(Info<id>) { return Type{}; }
	};

	template<typename T>
	constexpr auto meta = Type<T>::id;

	template<auto meta>
	using TypeOf = typename decltype(get(Info<meta>{}))::value_type;

	constexpr auto FindIndex(auto t, const std::ranges::range auto& ts) -> std::size_t
	{
		for (size_t i = 0; i < size(ts); i++)
		{
			if (ts[i] == t)
			{
				return i;
			}
		}
		return size(ts);
	}

	template<typename T, typename... Ts>
	constexpr std::size_t FindIndexMeta()
	{
		return FindIndex(meta<T>, std::array{ meta<Ts>... });
	}

	template<std::size_t Index, typename... Ts>
	struct TypeAt
	{
		using type = TypeOf < std::array{ meta<Ts>... }[Index] > ;
	};

	template<typename A, template<typename...> typename B> struct mp_rename_impl;

	template<template<typename...> typename A, typename... T, template<typename...> typename B>
	struct mp_rename_impl<A<T...>, B>
	{
		using type = B<T...>;
	};

	template<typename A, template<typename...> typename B>
	using mp_rename = typename mp_rename_impl<A, B>::type;

	template<class L> struct mp_size_impl;

	template<template<class...> class L, class... T> struct mp_size_impl<L<T...>>
	{
		using type = std::integral_constant<std::size_t, sizeof...(T)>;
	};

	template<class L> using mp_size = typename mp_size_impl<L>::type;

	template <typename... Types>
	concept EmptyParams = sizeof...(Types) == 0;

	template<typename T>
	concept Decayed = std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, T>;

	template<typename T>
	concept PointerType = std::is_pointer_v<T>;

	template<typename T>
	concept NotPointerType = !PointerType<T>;

	template<typename T>
	concept Fundamental = std::is_fundamental_v<T>;

	// Primary template: assumes T is not a specialization of the given class template
	template<template<typename...> typename Template, typename T>
	struct is_specialization_of : std::false_type {};

	// Specialization: detects if T is a specialization of the given class template
	template<template<typename...> typename Template, typename... Args>
	struct is_specialization_of<Template, Template<Args...>> : std::true_type {};

	// Helper variable template for convenience
	template<template<typename...> typename Template, typename T>
	constexpr bool is_specialization_of_v = is_specialization_of<Template, T>::value;

	template<typename T, template<typename...>  typename TemplateType>
	concept SpecializationOf = is_specialization_of_v<T, TemplateType>;

	template<int Value, int Min, int Max>
	concept InRange = Value >= Min && Value <= Max;

	template<typename T>
	struct OfTemplateType : std::false_type {};

	template<template <typename...> typename Base, typename... Ts>
	struct OfTemplateType<Base<Ts...>> : std::true_type {};

	template<template <typename...> typename Base, typename T>
	concept SameAsTemplateType = OfTemplateType<T>::value;

	template<typename T, size_t Size>
	concept SizeEqual = (sizeof(T) == Size);

	template<typename T, size_t Size>
	concept SizeNotEqual = (sizeof(T) != Size);

	template<typename T, size_t Size>
	concept SizeGreaterEqual = (sizeof(T) >= Size);

	template<typename T, size_t Size>
	concept SizeLessEqual = (sizeof(T) <= Size);

	template<typename T, size_t Size>
	concept SizeGreater = (sizeof(T) > Size);

	template<typename T, size_t Size>
	concept SizeLess = (sizeof(T) < Size);

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

	template<typename T, typename Return, typename... Args>
	concept HasOperator_Functor = requires(T a, Args... aArgs)
	{
		{ a(aArgs...) } -> std::same_as<Return>;
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
	concept IsPredicate = ReturnsType<T, bool, Args...>;

	template<typename T, typename Base>
	concept IsBaseOf = std::is_base_of_v<Base, T>;

	template<typename T, typename... Args>
	concept HasArgsConstructor = requires(Args&&... args)
	{
		T(std::forward<Args>(args)...);
	};

	template<typename T, typename ReturnType, typename... Args>
	concept ValidCallable = requires(T a, Args&&... inputs)
	{
		{ a(std::forward<Args>(inputs)...) } -> std::same_as<ReturnType>;
	};

	template<typename T, typename... Inputs>
	concept ValidCallableArgs = requires(T a, Inputs&&... inputs)
	{
		a(std::forward<Inputs>(inputs)...);
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

	// Define a trait to check if a type is only a raw reference
	template <typename T>
	struct S_IsRawReference : std::false_type {};

	template <typename T>
	struct S_IsRawReference<const T&> : std::false_type {};

	// Specialize for raw references (T&)
	template <typename T>
	struct S_IsRawReference<T&> : std::true_type {};

	// Specialize for rvalue references (T&&)
	template <typename T>
	struct S_IsRawReference<T&&> : std::true_type {};

	template<typename T>
	constexpr bool S_IsRawReference_T = S_IsRawReference<T>::value;

	template<typename T>
	concept IsRawReference = S_IsRawReference_T<T>;

	template<typename... Args>
	concept AnyArgIsRawReference = (IsRawReference<Args> || ...);

	template<typename T, typename... Types>
	concept ContainsType = (std::same_as<T, std::decay_t<Types>> || ...);

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

	template<typename T, typename First, typename... Rest>
	constexpr decltype(auto) Extract_Impl(First&& aFirst, [[maybe_unused]] Rest&&... aRest)
	{
		if constexpr (std::same_as<T, std::decay_t<First>>)
		{
			return std::forward<First>(aFirst);
		}
		else
		{
			return Extract_Impl<T>(std::forward<Rest>(aRest)...);
		}
	}

	template<typename T>
	constexpr decltype(auto) Extract_Impl()
	{
		//static_assert(false);
		return T{};
	}

	template<typename T, typename... Types> requires ContainsType<T, Types...>
	constexpr decltype(auto) Extract(Types&&... aTypes)
	{
		return Extract_Impl<T, Types...>(std::forward<Types>(aTypes)...);
	}


	template<typename T, typename... Types>
	constexpr decltype(auto) TryExtract(const T& aDefaultValue, [[maybe_unused]] Types&&... aTypes)
	{
		if constexpr (ContainsType<T, Types...>)
		{
			return Extract_Impl<T, Types...>(std::forward<Types>(aTypes)...);
		}
		else
		{
			return aDefaultValue;
		}
	}

	template<typename, template <typename...> typename>
	struct is_instance_of : std::false_type {};

	template<typename... Args, template <typename...> typename Template>
	struct is_instance_of<Template<Args...>, Template> : std::true_type {};

	// Convenience variable template
	template<typename T, template <typename...> typename Template>
	inline constexpr bool is_instance_of_v = is_instance_of<T, Template>::value;

	template<typename T, template <typename...> typename Template>
	concept SameAsTemplate = is_instance_of_v<T, Template>;

	template<template<typename> typename TemplateType, typename... Types>
	concept ContainsTemplateType = (SameAsTemplate<Types, TemplateType> || ...);

	template<template<typename> typename T, typename First, typename... Rest>
	constexpr decltype(auto) ExtractTemplate(First&& aFirst, [[maybe_unused]] Rest&&... aRest)
	{
		if constexpr (SameAsTemplate<First, T>)
		{
			return std::forward<First>(aFirst);
		}
		else
		{
			return ExtractTemplate<T>(std::forward<Rest>(aRest)...);
		}
	}

	template<typename, typename = void>
	constexpr bool is_type_complete_v = false;

	template<typename T>
	constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;

	template<typename T>
	concept TypeDefined = is_type_complete_v<T>;

	template<typename T>
	concept ViewAndEditable = requires(T & aValue)
	{
		{ ViewAndEdit(aValue) } -> std::same_as<ViewAndEditResult>;
	};

	template<typename T>
	concept Viewable = requires(const T & aValue)
	{
		{ View(aValue) } -> std::same_as<void>;
	};

	template<typename T>
	concept GlobalViewable = requires(const T & aValue)
	{
		{ ::View(aValue) } -> std::same_as<void>;
	};

	template<typename T, typename SerializationObject>
	concept Savable = requires(const T & aValue, SerializationObject & aSerializer)
	{
		{ Save(aValue, aSerializer) } -> std::same_as<void>;
	};

	template<typename T, typename Serializer>
	concept Loadable = requires(T & aValue, const Serializer & aSerializer)
	{
		{ Load(aValue, aSerializer) } -> std::same_as<void>;
	};

	template<typename T>
	concept DefaultConstructible = std::default_initializable<T>;

	template<typename T>
	concept NotDefaultConstructible = !DefaultConstructible<T>;

	template<typename T>
	concept Copyable = std::copyable<T>;

	template<typename T, typename SerializationObject>
	concept Scriptable =
		Decayed<T> &&
		ViewAndEditable<T> &&
		Savable<T, SerializationObject>&&
		Loadable<T, SerializationObject>&&
		DefaultConstructible<T>&&
		Copyable<T>;

	template<typename Type, typename... Types>
	constexpr bool PackContains()
	{
		return std::_Is_any_of_v<Type, Types...>;
	}

	template<typename ClassType, typename PropertyType>
	constexpr size_t GetByteOffset(PropertyType ClassType::* aProperty)
	{
		constexpr ClassType* a = nullptr;
		return (size_t) & reinterpret_cast<const char&>(a->*aProperty);
	}


	template<typename... Fs>
	struct Visitor final : Fs...
	{
		using Fs::operator()...;
	};

}