#pragma once
#include "ScriptDefines.h"
#include <vector>
#include <cassert>

namespace FLY_NAMESPACE
{
	template<typename IDType>
	class IDContainer
	{
	public:
		IDContainer(size_t aMaxSize = std::numeric_limits<size_t>::max());
		~IDContainer();

		bool PushBack(IDType anID);
		void Clear();

		IDType& operator[](size_t anIndex);
		const IDType& operator[](size_t anIndex) const;

		void SetMaxSize(size_t aMaxSize);

	private:

		std::vector<IDType> myIDs;

		size_t myMaxSize;
	};

	template<typename IDType>
	inline IDContainer<IDType>::IDContainer(size_t aMaxSize)
		: myMaxSize(aMaxSize)
	{
	}

	template<typename IDType>
	inline IDContainer<IDType>::~IDContainer()
	{
	}

	template<typename IDType>
	inline bool IDContainer<IDType>::PushBack(IDType anID)
	{
		bool canAdd = myIDs.size() < myMaxSize;

		if (canAdd)
		{
			myIDs.push_back(anID);
		}

		return canAdd;
	}

	template<typename IDType>
	inline void IDContainer<IDType>::Clear()
	{
		myIDs.clear();
	}
	template<typename IDType>
	inline IDType& IDContainer<IDType>::operator[](size_t anIndex)
	{
		return myIDs[anIndex];
	}
	template<typename IDType>
	inline const IDType& IDContainer<IDType>::operator[](size_t anIndex) const
	{
		return myIDs[anIndex];
	}
	template<typename IDType>
	inline void IDContainer<IDType>::SetMaxSize(size_t aMaxSize)
	{
		assert(myIDs.size() < aMaxSize);
		myMaxSize = aMaxSize;
	}
}