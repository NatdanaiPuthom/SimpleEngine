#pragma once
#include "../ScriptDefines.h"
#include <vector>

namespace SCR
{

	template<typename T>
	struct FunctionWrapper;

	template<typename Ret, typename... Args>
	struct FunctionWrapper<Ret(Args...)>
	{
		Ret(*func)(Args...);

		Ret operator()(Args... args) const
		{
			return func(std::forward<Args>(args)...);
		}

		operator bool() const
		{
			return func;
		}
	};

	enum class eBitwiseType
	{
		HasFlag,
		Equals
	};

	template<typename T>
	concept Enumerable = std::is_enum_v<T>;


	template<Enumerable T>
	constexpr auto EnumCast(T aValue)
	{
		return static_cast<std::underlying_type_t<T>>(aValue);
	}

	template<Enumerable T>
	constexpr T operator&(const T aValue1, const T aValue2)
	{
		return static_cast<T>(static_cast<std::underlying_type_t<T>>(aValue1) & static_cast<std::underlying_type_t<T>>(aValue2));
	}

	template<Enumerable T>
	constexpr T operator|(const T aValue1, const T aValue2)
	{
		return static_cast<T>(static_cast<std::underlying_type_t<T>>(aValue1) | static_cast<std::underlying_type_t<T>>(aValue2));
	}

	template<Enumerable T>
	constexpr T operator^(const T aValue1, const T aValue2)
	{
		return static_cast<T>(static_cast<std::underlying_type_t<T>>(aValue1) ^ static_cast<std::underlying_type_t<T>>(aValue2));
	}

	template<Enumerable T>
	constexpr void operator|=(T& aValue1, const T aValue2)
	{
		aValue1 = aValue1 | aValue2;
	}

	template<Enumerable T>
	constexpr bool HasFlag(const T aValue1, const T aValue2)
	{
		return std::underlying_type_t<T>(aValue1 & aValue2) > 0;
	}

	template<Enumerable T>
	constexpr bool HasNotFlag(const T aValue1, const T aValue2)
	{
		return !HasFlag(aValue1, aValue2);
	}

	template<Enumerable T>
	constexpr bool Equals(const T aValue1, const T aValue2)
	{
		return aValue1 == aValue2;
	}

	template<Enumerable T>
	constexpr std::underlying_type_t<T> EnumToInt(const T aValue)
	{
		return static_cast<std::underlying_type_t<T>>(aValue);
	}


	template<typename CountType = size_t, typename T, typename Callable, typename... Args>
	void For(std::vector<T>& someObjects, Callable&& aCallable, Args&&... args)
	{
		for (CountType i = 0; i < someObjects.size(); ++i)
		{
			aCallable(i, someObjects[i], std::forward<Args>(args)...);
		}
	}

	template<typename CountType = size_t, typename T, typename Callable, typename... Args>
	void For_Break(std::vector<T>& someObjects, Callable&& aCallable, Args&&... args)
	{
		for (CountType i = 0; i < someObjects.size(); ++i)
		{
			if (aCallable(i, someObjects[i], std::forward<Args>(args)...))
			{
				break;
			}
		}
	}

	template<typename CountType = size_t, typename T, typename Callable, typename... Args>
	void For_Const(const std::vector<T>& someObjects, Callable&& aCallable, Args&&... args)
	{
		for (CountType i = 0; i < someObjects.size(); ++i)
		{
			aCallable(i, someObjects[i], std::forward<Args>(args)...);
		}
	}

	template<typename CountType = size_t, typename T, typename Callable, typename... Args>
	void For_Const_Break(const std::vector<T>& someObjects, Callable&& aCallable, Args&&... args)
	{
		for (CountType i = 0; i < someObjects.size(); ++i)
		{
			if (aCallable(i, someObjects[i], std::forward<Args>(args)...))
			{
				break;
			}
		}
	}

	template<typename CountType = size_t, typename Callable, typename... Args>
	void For_Pure(CountType aNumber, Callable&& aCallable, Args&&... args)
	{
		for (CountType i = 0; i < aNumber; ++i)
		{
			aCallable(std::forward<Args>(args)...);
		}
	}

	template<typename T>
	class Stream
	{
		Stream() = default;
	public:

		Stream(const std::vector<T>& aVector)
		{
			for (const T& value : aVector)
			{
				myVector.push_back(&value);
			}
		}

		~Stream() = default;

		template<typename FilterFunction>
		Stream<T> Filter(FilterFunction&& aFilterFunction) const
		{
			Stream<T> newStream;

			for (const T* value : myVector)
			{
				if (aFilterFunction(*value))
				{
					newStream.myVector.push_back(value);
				}
			}

			return newStream;
		}

		size_t Size() const
		{
			return myVector.size();
		}

		std::vector<const T*> Get() const
		{
			return myVector;
		}

		std::vector<T> GetCopy() const
		{
			std::vector<T> copy;
			for (const T* value : myVector)
			{
				copy.push_back(*value);
			}
			return copy;
		}

	private:

		std::vector<const T*> myVector;
	};

	template<typename T, typename IndexType = size_t>
	class IndexStream
	{
		IndexStream(const T& (*aConverterFunction)(IndexType))
			: myConverterFunction(aConverterFunction)
		{

		}
	public:

		IndexStream(const std::vector<T>& aVector, const T& (*aConverterFunction)(IndexType))
			: myConverterFunction(aConverterFunction)
		{
			for (IndexType i = 0; i < aVector.size(); ++i)
			{
				myIndices.push_back(i);
			}
		}

		template<typename FilterFunction>
		IndexStream<T, IndexType> Filter(FilterFunction&& aFilterFunction)
		{
			IndexStream<T, IndexType> newStream(myConverterFunction);
			for (IndexType i : myIndices)
			{
				if (aFilterFunction(myConverterFunction(i)))
				{
					newStream.myIndices.push_back(i);
				}
			}

			return newStream;
		}



		const std::vector<IndexType>& Get() const
		{
			return myIndices;
		}


	private:

		std::vector<IndexType> myIndices;
		const T& (*myConverterFunction) (IndexType);
	};
}