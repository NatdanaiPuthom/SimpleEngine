#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{

	class Foundation;
	class NodeExecutor;
	class DataTypeManager;
	class NodeTypeManager;
	class PinTypeManager;
	class MemoryPool;
	template<size_t> class MemoryArena;
	class Class;

	namespace Global
	{


		Foundation& GetFoundation();
		NodeExecutor& GetNodeExecutor();
		DataTypeManager& GetDataTypeManager();
		NodeTypeManager& GetNodeTypeManager();
		PinTypeManager& GetPinTypeManager();
		Class& GetClassByID(ClassID aClassID);

		bool& IsDebugging();

		namespace Internal
		{
			MemoryArena<1024>& GetFrameMemoryArena();
			MemoryArena<10000>& GetEditMemoryArena();
			MemoryPool& GetMemoryPool();
			NodeGraph& GetNodeGraphCopy();
		}
	}


}