#include "FlyGlobal.hpp"
#include "FlyFoundation.hpp"
#include "DataType/FlyDataTypeManager.hpp"
#include "FlyNodeTypeManager.hpp"
#include "FlyPinTypeManager.hpp"
#include "Type/FlyTypeManager.hpp"
#include "../FlyCopyBuffer.hpp"

namespace FLY_NAMESPACE
{
	namespace Global
	{

		bool g_sDebugging = false;
		MemoryArena<1024> g_FrameArena;
		MemoryArena<10000> g_EditArena;
		CopyBuffer g_CopyBuffer;

		Foundation& GetFoundation()
		{
			return Foundation::GetInstance();
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

		Class& GetClassByID(const ClassID aID)
		{
			return GetFoundation().GetClassByID(aID);
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

