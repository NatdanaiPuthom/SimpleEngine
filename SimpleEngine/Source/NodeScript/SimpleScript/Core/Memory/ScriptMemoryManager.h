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

		template<typename T, typename... Args>
		T& Allocate(Args&&... aArgs)
		{
			return myMemory.Allocate<T>(std::forward<Args>(aArgs)...);
		}

		const MemoryArena<1024>& GetMemory() const
		{
			return myMemory;
		}

		void Clear()
		{
			return myMemory.Clear();
		}


	private:


		MemoryArena<1024> myMemory;


	};
}