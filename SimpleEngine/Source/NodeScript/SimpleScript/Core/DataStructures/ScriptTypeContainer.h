#pragma once
#include "../ScriptDefines.h"
#include <functional>

namespace SCR
{
	
	template<typename IDType, typename T, typename Type>
	class TypeContainer
	{
	public:
		class ConstIterator
		{
			using Tuple = std::tuple<IDType, const T*, const Type*>;
		public:
			ConstIterator(const IDType anID, const T* aObject, const TypeContainer& aContainer)
				: myTuple(Tuple{ anID, aObject, nullptr })
				, myContainer(aContainer)
			{
				if (anID < myContainer.myObjects.size())
				{
					std::get<2>(myTuple) = &aContainer.myToTypeConverter(*aObject);
				}
			}
			~ConstIterator()
			{

			}

			const Tuple& operator*()
			{
				return myTuple;
			}

			ConstIterator& operator++()
			{
				IDType& id = std::get<0>(myTuple);
				const T*& object = std::get<1>(myTuple);
				const Type*& objectType = std::get<2>(myTuple);
				++id;
				if (id < myContainer.myObjects.size())
				{
					object = &myContainer.myObjects[id];
					objectType = &myContainer.myToTypeConverter(*object);
				}
				return *this;
			}

			bool operator==(const ConstIterator& aOther) const
			{
				return std::get<0>(myTuple) == std::get<0>(aOther.myTuple);
			}

			bool operator!=(const ConstIterator& aOther) const
			{
				return !(*this == aOther);
			}


		private:

			Tuple myTuple;
			const TypeContainer myContainer;
		};

	public:

		
		TypeContainer(const std::vector<T>& aObjects, const std::function<const Type&(const T&)>& aToTypeConverter);
		~TypeContainer();

		ConstIterator begin() const
		{
			if (myObjects.empty())
			{
				return ConstIterator(0, nullptr, *this);
			}
			return ConstIterator(0, &myObjects[0], *this);
		}
		ConstIterator end() const
		{
			return ConstIterator(static_cast<IDType>(myObjects.size()), nullptr, *this);
		}

	private:


		const std::vector<T>& myObjects;
		std::function<const Type&(const T&)> myToTypeConverter;
	};

	template<typename IDType, typename T, typename Type>
	inline TypeContainer<IDType, T, Type>::TypeContainer(const std::vector<T>& aObjects, const std::function<const Type&(const T&)>& aToTypeConverter)
		: myObjects(aObjects)
		, myToTypeConverter(aToTypeConverter)
	{
	}
	template<typename IDType, typename T, typename Type>
	inline TypeContainer<IDType, T, Type>::~TypeContainer()
	{
	}
}