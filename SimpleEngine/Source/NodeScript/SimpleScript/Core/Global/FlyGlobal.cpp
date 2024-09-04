#include "FlyGlobal.hpp"
#include "FlyFoundation.hpp"
#include "DataType/FlyDataTypeManager.hpp"
#include "FlyNodeTypeManager.hpp"
#include "FlyPinTypeManager.hpp"
#include "Type/FlyTypeManager.hpp"
#include "../Graph/FlyNodeGraph.hpp"

namespace FLY_NAMESPACE
{
	namespace Global
	{

		bool g_IsDebugging = false;
		MemoryArena<1024> g_FrameArena;
		MemoryArena<10000> g_EditArena;

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

		bool& IsDebugging()
		{
			return g_IsDebugging;
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

			MemoryPool& GetMemoryPool()
			{
				return GetFoundation().mMemoryPool;
			}

			EventGraph& GetNodeGraphCopy()
			{
				return GetFoundation().mNodeGraphCopy;
			}
		}
	}

}

