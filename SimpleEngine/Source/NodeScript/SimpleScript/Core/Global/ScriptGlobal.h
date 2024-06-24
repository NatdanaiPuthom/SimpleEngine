#pragma once
#include "../ScriptDefines.h"

namespace SCR
{

	class ScriptFoundation;
	class DataTypeManager;
	class NodeTypeManager;
	class PinTypeManager;
	class MemoryManager;

	namespace Global
	{


		ScriptFoundation& GetFoundation();
		DataTypeManager& GetDataTypeManager();
		NodeTypeManager& GetNodeTypeManager();
		PinTypeManager& GetPinTypeManager();

		bool& IsDebugging();

		namespace Internal
		{
			MemoryManager& GetFrameBuffer();
			MemoryManager& GetEditBuffer();
		}
	}


}