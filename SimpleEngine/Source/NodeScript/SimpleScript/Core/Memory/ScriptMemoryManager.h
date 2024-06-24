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

		const MemoryArena<NodeBufferCapacity>& GetMemory() const
		{
			return myMemory;
		}

		MemoryArena<NodeBufferCapacity>& GetMemory()
		{
			return myMemory;
		}

		void Clear()
		{
			return myMemory.Clear();
		}


	private:


		MemoryArena<NodeBufferCapacity> myMemory;


	};
}