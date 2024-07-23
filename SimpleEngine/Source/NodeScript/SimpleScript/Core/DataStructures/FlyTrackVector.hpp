#pragma once
#include "../ScriptDefines.h"
#include "../SystemTypes/ReferenceWrapper.h"

namespace FLY_NAMESPACE
{

	template<typename... Types>
	class TrackVector final
	{
		static constexpr size_t Amount = sizeof...(Types);
		using Structured = std::tuple<ReferenceWrapper<Types>...>;
		using StructuredConst = std::tuple<std::add_const<std::add_pointer<Types>>...>;
	public:

		TrackVector()
		{

		}

		TrackVector(size_t aSize)
		{
			Iterate([aSize](auto& aVector) -> void { aVector.resize(aSize); });
		}

		~TrackVector()
		{

		}

		constexpr Structured operator[](size_t anIndex)
		{
			Structured s;
			Populate(s, anIndex);
			return s;
		}

		constexpr size_t Size() const
		{
			return First().size();
		}


	private:

		auto First() const
		{
			return std::get<0>(myVectors);
		}

		template<typename Callable>
		void Iterate(Callable&& aCallable)
		{
			IterateImpl<0>(aCallable);
		}

		template<size_t Index, typename Callable>
		void IterateImpl(Callable&& aCallable)
		{
			aCallable(std::get<Index>(myVectors));

			if constexpr (Index + 1 < Amount)
			{
				IterateImpl<Index + 1>(std::forward<Callable>(aCallable));
			}
		}

		void Populate(Structured& aStructured, size_t anIndex)
		{
			PopulateImpl<0>(aStructured, anIndex);
		}

		template<size_t Index>
		void PopulateImpl(Structured& aStructured, size_t anIndex)
		{
			std::get<Index>(aStructured) = myVectors.at(anIndex);
		}
	private:


		std::tuple<std::vector<Types>...> myVectors;

	};
}