#include "ScriptGlobal.h"
#include "ScriptFoundation.h"
#include "DataType/DataTypeManager.h"
#include "NodeTypeManager.h"
#include "PinTypeManager.h"
#include "Type/ScriptTypeManager.h"
#include "../ScriptCopyBuffer.h"

namespace SCR
{
	namespace Global
	{

		bool g_sDebugging = false;
		MemoryArena<1024> g_FrameArena;
		MemoryArena<10000> g_EditArena;
		CopyBuffer g_CopyBuffer;

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
			MemoryArena<1024>& GetFrameMemoryArena()
			{
				return g_FrameArena;
			}

			MemoryArena<10000>& GetEditMemoryArena()
			{
				return g_EditArena;
			}
			CopyBuffer& GetCopyBuffer()
			{
				return g_CopyBuffer;
			}
		}
	}

}

