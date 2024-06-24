#include "ScriptGlobal.h"
#include "ScriptFoundation.h"
#include "DataType/DataTypeManager.h"
#include "NodeTypeManager.h"
#include "PinTypeManager.h"
#include "Type/ScriptTypeManager.h"

namespace SCR
{
	namespace Global
	{

		bool g_sDebugging = false;
		MemoryManager g_FrameBuffer;
		MemoryManager g_EditBuffer;

		ScriptFoundation& GetFoundation()
		{
			return ScriptFoundation::GetInstance();
		}

		DataTypeManager& GetDataTypeManager()
		{
			return GetFoundation().GetTypeManager().GetDataTypeManager();
		}

		NodeTypeManager& GetNodeTypeManager()
		{
			return GetFoundation().GetTypeManager().GetNodeTypeManager();
		}

		PinTypeManager& GetPinTypeManager()
		{
			return GetFoundation().GetTypeManager().GetPinTypeManager();
		}

		bool& IsDebugging()
		{
			return g_sDebugging;
		}

		namespace Internal
		{
			MemoryManager& GetFrameBuffer()
			{
				return g_FrameBuffer;
			}

			MemoryManager& GetEditBuffer()
			{
				return g_EditBuffer;
			}
		}
	}

}

