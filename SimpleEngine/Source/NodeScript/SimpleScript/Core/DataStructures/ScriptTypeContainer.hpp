#pragma once
#include "../ScriptDefines.hpp"
#include <functional>

namespace SCR
{
	
	template<typename IDType, typename T, typename Type>
	class TypeContainer
	{
	public:
		class Iterator
		{
			using Tuple = std::tuple<IDType, T*, Type*>;
		public:
			Iterator(const IDType anID, T* aObject, const TypeContainer& aContainer)
				: myTuple(Tuple{ anID, aObject, nullptr })
				, myContainer(aContainer)
			{
				if (anID < myContainer.myObjects.size())
				{
					std::get<2>(myTuple) = &aContainer.myToTypeConverter(*aObject);
				}
			}
			~Iterator()
			{

			}

			const Tuple& operator*()
			{
				return myTuple;
			}

			Iterator& operator++()
			{
				IDType& id = std::get<0>(myTuple);
				T*& object = std::get<1>(myTuple);
				Type*& objectType = std::get<2>(myTuple);
				++id;
				if (id < myContainer.myObjects.size())
				{
					object = &myContainer.myObjects[id];
					objectType = &myContainer.myToTypeConverter(*object);
				}
				return *this;
			}

			bool operator==(const Iterator& aOther) const
			{
				return std::get<0>(myTuple) == std::get<0>(aOther.myTuple);
			}

			bool operator!=(const Iterator& aOther) const
			{
				return !(*this == aOther);
			}


		private:

			Tuple myTuple;
			const TypeContainer myContainer;
		};


	public:

		using ConverterFunction = std::function<Type& (const T&)>;
		
		TypeContainer(std::vector<T>& aObjects, const ConverterFunction& aToTypeConverter)
			: myObjects(aObjects)
			, myToTypeConverter(aToTypeConverter)
		{
		}
		~TypeContainer()
		{

		}

		Iterator begin() const
		{
			if (myObjects.empty())
			{
				return Iterator(0, nullptr, *this);
			}
			return Iterator(0, &myObjects[0], *this);
		}
		Iterator end() const
		{
			return Iterator(static_cast<IDType>(myObjects.size()), nullptr, *this);
		}

	private:


		std::vector<T>& myObjects;
		ConverterFunction myToTypeConverter;
	};

	template<typename IDType, typename T, typename Type>
	class TypeContainer<IDType, const T, const Type>
	{
	public:
		class Iterator
		{
			using Tuple = std::tuple<IDType, const T*, const Type*>;
		public:
			Iterator(const IDType anID, const T* aObject, const TypeContainer& aContainer)
				: myTuple(Tuple{ anID, aObject, nullptr })
				, myContainer(aContainer)
			{
				if (anID < myContainer.myObjects.size())
				{
					std::get<2>(myTuple) = &aContainer.myToTypeConverter(*aObject);
				}
			}
			~Iterator()
			{

			}

			const Tuple& operator*()
			{
				return myTuple;
			}

			Iterator& operator++()
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

			bool operator==(const Iterator& aOther) const
			{
				return std::get<0>(myTuple) == std::get<0>(aOther.myTuple);
			}

			bool operator!=(const Iterator& aOther) const
			{
				return !(*this == aOther);
			}


		private:

			Tuple myTuple;
			const TypeContainer myContainer;
		};


	public:

		using ConverterFunction = std::function<const Type& (const T&)>;

		TypeContainer(const std::vector<T>& aObjects, const ConverterFunction& aToTypeConverter)
			: myObjects(aObjects)
			, myToTypeConverter(aToTypeConverter)
		{
		}

		~TypeContainer()
		{

		}

		Iterator begin() const
		{
			if (myObjects.empty())
			{
				return end();
			}
			return Iterator(0, &myObjects[0], *this);
		}
		Iterator end() const
		{
			return Iterator(static_cast<IDType>(myObjects.size()), nullptr, *this);
		}

	private:


		const std::vector<T>& myObjects;
		ConverterFunction myToTypeConverter;
	};
}