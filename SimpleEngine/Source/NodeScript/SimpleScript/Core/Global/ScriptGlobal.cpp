#include "ScriptGlobal.hpp"
#include "ScriptFoundation.hpp"
#include "DataType/DataTypeManager.hpp"
#include "NodeTypeManager.hpp"
#include "PinTypeManager.hpp"
#include "Type/ScriptTypeManager.hpp"
#include "../ScriptCopyBuffer.hpp"

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

		NodeExecutor& GetNodeExecutor()
		{
			return GetFoundation().GetNodeExecutor();
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

