#pragma once
#include "../FlyDefines.hpp"
#include <functional>

namespace FLY_NAMESPACE
{
	template<typename... Args>
	class Predicate final
	{
	public:

		Predicate() = default;

		template<typename Func>
		Predicate(Func&& aPredicateFunction)
			: mPredicateFunction(std::move(aPredicateFunction))
		{
		}

		bool operator()(Args... aArgs) const
		{
			return mPredicateFunction(std::forward<Args>(aArgs)...);
		}

		explicit operator bool() const
		{
			return mPredicateFunction.operator bool();
		}

	private:

		std::function<bool(Args...)> mPredicateFunction;
	};
}