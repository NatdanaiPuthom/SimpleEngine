#pragma once
#include "../ScriptDefines.h"
#include "ScriptMemoryPool.h"

namespace SCR
{
	class MemoryManager final
	{
	public:

		MemoryManager() = default;
		~MemoryManager() = default;

		template<typename T>
		T& Allocate(const T& aDefaultValue = T())
		{
			return myMemory.Allocate<T>(aDefaultValue);
		}

	private:


		MultiMemoryPool<1024> myMemory;


	};
}