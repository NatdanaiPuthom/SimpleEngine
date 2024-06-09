#pragma once
#include "../ScriptDefines.h"
#include "../Utilities/MetaScript.h"
#include "SystemTypes/ReferenceWrapper.h"

namespace SCR
{

	template<typename...>
	struct FunctionTypeList {};

	template<typename... Args>
	using ReturnList = FunctionTypeList<Args...>;

	template<typename... Args>
	using ConstantList = FunctionTypeList<Args...>;

	template<typename... Args>
	using VariedList = FunctionTypeList<Args...>;

	template<typename Returns, typename ConstantArgs, typename VariedArgs>
	class AccessorTuple;

	template<typename... ConstantArgs, typename... VariedArgs> requires NoArgsReference<ConstantArgs...>
	class AccessorTuple<ReturnList<>, ConstantList<ConstantArgs...>, VariedList<VariedArgs...>>
	{
	public:

		using VariedArgsType = std::array<std::tuple<VariedArgs...>, 0>;


		constexpr size_t Size() const
		{
			return 0;
		}


		template<typename Callable, typename... CallableArgs>
		decltype(auto) Call(Callable&& aCallable, ConstantArgs..., VariedArgsType&&, CallableArgs&&... aCallableArgs) const
		{
			return aCallable(std::forward<CallableArgs>(aCallableArgs)...);
		}
	};

	template<typename... Returns, typename... ConstantArgs, typename... VariedArgs> requires NoArgsReference<ConstantArgs...>
	class AccessorTuple<ReturnList<Returns...>, ConstantList<ConstantArgs...>, VariedList<VariedArgs...>>
	{
	private:

		template<typename ReturnType>
		using CallSignature = ReturnType&(*)(ConstantArgs..., VariedArgs...);

		std::tuple<CallSignature<Returns>...> myFunctions{};

	public:

		using ResultTuple = std::tuple<ReferenceWrapper<Returns>...>;

		using VariedArgsType = std::array<std::tuple<VariedArgs...>, sizeof...(Returns)>;


		template<size_t Index> requires IsInRange<Index, 0, static_cast<int>(sizeof...(Returns)) - 1>
		constexpr decltype(auto) At()
		{
			return std::get<Index>(myFunctions);
		}

		template<size_t Index> requires IsInRange<Index, 0, static_cast<int>(sizeof...(Returns)) - 1>
		constexpr decltype(auto) At() const
		{
			return std::get<Index>(myFunctions);
		}

		constexpr size_t Size() const
		{
			return sizeof...(Returns);
		}

		template<typename Callable, typename... CallableArgs>
		decltype(auto) Call(Callable&& aCallable, ConstantArgs... aConstantArgs, VariedArgsType&& aVariedArgs, CallableArgs&&... aCallableArgs) const
		{
			return std::apply(std::forward<Callable>(aCallable),
				std::tuple_cat(std::make_tuple<CallableArgs...>(
					std::forward<CallableArgs>(aCallableArgs)...),
					std::forward<ResultTuple>(
						Resolve(
							std::forward<ConstantArgs>(aConstantArgs)...,
							std::forward<VariedArgsType>(aVariedArgs)
						)
					)
				)
			);
		}

		/*template<typename Callable, typename... CallableArgs>
		decltype(auto) Call(Callable&& aCallable, ConstantArgs... aConstantArgs, CallableArgs&&... aCallableArgs) const
		{
			return std::apply(std::forward<Callable>(aCallable),
				std::tuple_cat(
					std::make_tuple<CallableArgs...>(std::forward<CallableArgs>(aCallableArgs)...), 
					std::forward<ResultTuple>(Resolve(std::forward<ConstantArgs>(aConstantArgs)...))
				)
			);
		}*/

		ResultTuple Resolve(ConstantArgs&&... aConstantArgs, VariedArgsType&& aVariedArgs = VariedArgsType()) const
		{
			ResultTuple resultTuple;
			ResolveImpl(std::index_sequence_for<Returns...>(), resultTuple, std::forward<ConstantArgs>(aConstantArgs)..., std::forward<VariedArgsType>(aVariedArgs));
			return resultTuple;
		}

	private:

		template<std::size_t... Indices>
		void ResolveImpl(std::index_sequence<Indices...>, ResultTuple& aResultTuple, ConstantArgs&&... aConstantArgs, VariedArgsType&& aVariedArgs) const
		{
			(ResolveAt<Indices>(aResultTuple, std::forward<ConstantArgs>(aConstantArgs)..., std::forward<VariedArgsType>(aVariedArgs)), ...);
		}

		template<size_t Index>
		void ResolveAt(ResultTuple& aResultTuple, ConstantArgs&&... aConstantArgs, VariedArgsType&& aVariedArgs) const
		{
			std::get<Index>(aResultTuple) = std::apply(std::get<Index>(myFunctions),
				std::tuple_cat(
					std::make_tuple(std::forward<ConstantArgs>(aConstantArgs)...),
					std::get<Index>(std::forward<VariedArgsType>(aVariedArgs))
				)
			);
		}

	};


	/// Testing
	template<typename Returns, typename ConstantArgs, typename VariedArgs>
	class AccessorTuple2;

	template<typename... Returns, typename... ConstantArgs, typename... VariedArgs>
	class AccessorTuple2<ReturnList<Returns...>, ConstantList<ConstantArgs...>, VariedList<VariedArgs...>>
	{
	private:

		template<typename ReturnType>
		using CallSignature = ReturnType& (*)(ConstantArgs..., VariedArgs...);

		std::tuple<CallSignature<Returns>...> myFunctions{};

	public:

		using ResultTuple = std::tuple<ReferenceWrapper<Returns>...>;

		using VariedArgsType = std::array<std::tuple<VariedArgs...>, sizeof...(Returns)>;


		template<size_t Index> requires IsInRange<Index, 0, static_cast<int>(sizeof...(Returns)) - 1>
		constexpr decltype(auto) At()
		{
			return std::get<Index>(myFunctions);
		}

		template<size_t Index> requires IsInRange<Index, 0, static_cast<int>(sizeof...(Returns)) - 1>
		constexpr decltype(auto) At() const
		{
			return std::get<Index>(myFunctions);
		}

		constexpr size_t Size() const
		{
			return sizeof...(Returns);
		}

		template<typename Callable, typename... CallableArgs>
		decltype(auto) Call(Callable&& aCallable, ConstantArgs... aConstantArgs, VariedArgsType&& aVariedArgs, CallableArgs&&... aCallableArgs) const
		{
			return std::apply(std::forward<Callable>(aCallable),
				std::tuple_cat(std::make_tuple<CallableArgs...>(
					std::forward<CallableArgs>(aCallableArgs)...),
					std::forward<ResultTuple>(
						Resolve(
							std::forward<ConstantArgs>(aConstantArgs)...,
							std::forward<VariedArgsType>(aVariedArgs)
						)
					)
				)
			);
		}

		template<typename Callable, typename... CallableArgs>
		decltype(auto) Call(Callable&& aCallable, ConstantArgs... aConstantArgs, CallableArgs&&... aCallableArgs) const
		{
			return std::apply(std::forward<Callable>(aCallable),
				std::tuple_cat(
					std::make_tuple<CallableArgs...>(std::forward<CallableArgs>(aCallableArgs)...),
					std::forward<ResultTuple>(Resolve(std::forward<ConstantArgs>(aConstantArgs)...))
				)
			);
		}

		ResultTuple Resolve(ConstantArgs&&... aConstantArgs, VariedArgsType&& aVariedArgs = VariedArgsType()) const
		{
			ResultTuple resultTuple;
			ResolveImpl(std::index_sequence_for<Returns...>(), resultTuple, std::forward<ConstantArgs>(aConstantArgs)..., std::forward<VariedArgsType>(aVariedArgs));
			return resultTuple;
		}

	private:

		template<std::size_t... Indices>
		void ResolveImpl(std::index_sequence<Indices...>, ResultTuple& aResultTuple, ConstantArgs&&... aConstantArgs, VariedArgsType&& aVariedArgs) const
		{
			(ResolveAt<Indices>(aResultTuple, std::forward<ConstantArgs>(aConstantArgs)..., std::forward<VariedArgsType>(aVariedArgs)), ...);
		}

		template<size_t Index>
		void ResolveAt(ResultTuple& aResultTuple, ConstantArgs&&... aConstantArgs, VariedArgsType&& aVariedArgs) const
		{
			std::get<Index>(aResultTuple) = std::apply(std::get<Index>(myFunctions),
				std::tuple_cat(
					std::make_tuple(std::forward<ConstantArgs>(aConstantArgs)...),
					std::get<Index>(std::forward<VariedArgsType>(aVariedArgs))
				)
			);
		}

	};

	class MemoryPool;

	template<typename... Returns>
	class MemoryTupleNew
	{

	private:

		template<typename ReturnType>
		using CallSignature = ReturnType & (*)(MemoryPool*, MemoryPoolID);

		std::tuple<CallSignature<Returns>...> myFunctions{};

	public:

		using ResultTuple = std::tuple<ReferenceWrapper<Returns>...>;
		using MemoryIDArray = std::array<MemoryPoolID, sizeof...(Returns)>;


		template<size_t Index> requires IsInRange<Index, 0, static_cast<int>(sizeof...(Returns)) - 1>
		constexpr decltype(auto) At()
		{
			return std::get<Index>(myFunctions);
		}

		template<size_t Index> requires IsInRange<Index, 0, static_cast<int>(sizeof...(Returns)) - 1>
		constexpr decltype(auto) At() const
		{
			return std::get<Index>(myFunctions);
		}

		constexpr size_t Size() const
		{
			return sizeof...(Returns);
		}

		template<typename Callable, typename... CallableArgs>
		decltype(auto) Call(Callable&& aCallable, MemoryPool* aMemoryPool, const MemoryIDArray& anIDArray, CallableArgs&&... aCallableArgs) const
		{
			return std::apply(std::forward<Callable>(aCallable),
				std::tuple_cat(std::make_tuple<CallableArgs...>(
					std::forward<CallableArgs>(aCallableArgs)...),
					std::forward<ResultTuple>(Resolve(aMemoryPool, anIDArray))
				)
			);
		}

		ResultTuple Resolve(MemoryPool* aMemoryPool, const MemoryIDArray& aMemoryIDArray) const
		{
			ResultTuple resultTuple;
			ResolveImpl(std::index_sequence_for<Returns...>(), resultTuple, aMemoryPool, aMemoryIDArray);
			return resultTuple;
		}

	private:

		template<std::size_t... Indices>
		void ResolveImpl(std::index_sequence<Indices...>, ResultTuple& aResultTuple, MemoryPool* aMemoryPool, const MemoryIDArray& aMemoryIDArray) const
		{
			(ResolveAt<Indices>(aResultTuple, aMemoryPool, aMemoryIDArray), ...);
		}

		template<size_t Index>
		void ResolveAt(ResultTuple& aResultTuple, MemoryPool* aMemoryPool, const MemoryIDArray& aMemoryIDArray) const
		{
			std::get<Index>(aResultTuple) = std::apply(std::get<Index>(myFunctions), std::tuple<MemoryPool*, MemoryPoolID>{ aMemoryPool, std::get<Index>(aMemoryIDArray) });
		}

	};

	template<typename... Args>
	class AccessorTupleNew
	{
	public:



		AccessorTupleNew()
		{

		}
		~AccessorTupleNew()
		{

		}

		const std::tuple<ReferenceWrapper<Args>...>& Get() const
		{
			return myData;
		}

		template<size_t Index> requires IsInRange<Index, 0, sizeof...(Args) - 1>
		constexpr decltype(auto) At()
		{
			return std::get<Index>(myData).GetPtr();
		}

		template<size_t Index> requires IsInRange<Index, 0, sizeof...(Args) - 1>
		constexpr decltype(auto) At() const
		{
			return std::get<Index>(myData).GetPtr();
		}


	private:

		std::tuple<ReferenceWrapper<Args>...> myData;
	};

}