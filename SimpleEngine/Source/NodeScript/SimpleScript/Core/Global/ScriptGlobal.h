#pragma once
#include "../ScriptDefines.h"

namespace SCR
{

	class ScriptFoundation;
	class DataTypeManager;
	class NodeTypeManager;
	class PinTypeManager;
	class MemoryManager;
	template<size_t> class MemoryArena;
	struct CopyBuffer;

	namespace Global
	{


		ScriptFoundation& GetFoundation();
		DataTypeManager& GetDataTypeManager();
		NodeTypeManager& GetNodeTypeManager();
		PinTypeManager& GetPinTypeManager();

		bool& IsDebugging();

		namespace Internal
		{
			MemoryArena<1024>& GetFrameMemoryArena();
			MemoryArena<10000>& GetEditMemoryArena();
			CopyBuffer& GetCopyBuffer();
		}
	}


}