#pragma once
#include "ScriptDefines.h"
#include "Memory/ScriptMemoryPool.h"
#include <memory>

namespace SCR
{

	class NodeManager;
	class PinManager;
	
	class ScriptGraph final
	{
		friend class ScriptProxy;
	public:

		ScriptGraph();
		~ScriptGraph();

	private:

		MemoryPool myMemoryPool;
		std::unique_ptr<NodeManager> myNodeManager;
		std::unique_ptr<PinManager> myPinManager;
	};
}