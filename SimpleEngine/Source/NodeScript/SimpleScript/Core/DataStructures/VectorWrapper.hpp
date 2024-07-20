#pragma once
#include "ScriptDefines.h"

namespace SCR
{

	template<CleanType T>
	struct VectorWrapper
	{
		std::vector<T> myValues;


		T& operator[](const size_t anIndex)
		{
			return myValues[anIndex];
		}

		const T& operator[](const size_t anIndex) const
		{
			return myValues[anIndex];
		}

		void operator++()
		{
			myValues.emplace_back();
		}

		size_t Size() const
		{
			return myValues.size();
		}

	};
}