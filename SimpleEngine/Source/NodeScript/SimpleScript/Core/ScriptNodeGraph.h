#pragma once
#include "ScriptDefines.h"
#include "NodeManager.h"
#include "PinManager.h"
#include "Memory/ScriptMemoryManager.h"

namespace SCR
{

	class NodeManager;
	class PinManager;

	class NodeGraph final
	{
		friend class ScriptProxy;
	public:

		NodeGraph();
		~NodeGraph();

		NodeGraph(const NodeGraph&);
		NodeGraph(NodeGraph&&) = default;

		NodeGraph& operator=(const NodeGraph&) = delete;
		NodeGraph& operator=(NodeGraph&&) = delete;

	public:

		std::unique_ptr<NodeManager> myNodeManager;
		std::unique_ptr<PinManager> myPinManager;

		MemoryManager myMemoryManager;
		//MemoryPool myMemoryPool;
	};
}