#pragma once
#include "../ScriptDefines.h"
#include "ScriptMemoryArena.h"

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

		const MemoryArena<1024>& GetMemory() const
		{
			return myMemory;
		}

	private:


		MemoryArena<1024> myMemory;


	};
}