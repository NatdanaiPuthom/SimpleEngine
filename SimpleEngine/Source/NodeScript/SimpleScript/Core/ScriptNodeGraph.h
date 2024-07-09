#pragma once
#include "ScriptDefines.h"
#include "Node/NodeManager.h"
#include "Pin/PinManager.h"
#include "Memory/ScriptMemoryArena.h"

namespace SCR
{

	class NodeManager;
	class PinManager;

	enum class eNodeGraphType
	{
		Event,
		Function,
		Other
	};

	class NodeGraph
	{
		friend class ScriptProxy;
	public:

		NodeGraph(eNodeGraphType aType);
		virtual ~NodeGraph();

		NodeGraph(const NodeGraph&);
		NodeGraph(NodeGraph&&) = default;

		NodeGraph& operator=(const NodeGraph&) = delete;
		NodeGraph& operator=(NodeGraph&&) = default;

		eNodeGraphType GetType() const;

	public:

		std::unique_ptr<NodeManager> myNodeManager;
		std::unique_ptr<PinManager> myPinManager;
		std::vector<Link> myLinks;

		MemoryArena<NodeBufferCapacity> myMemoryArena;

	private:

		eNodeGraphType myType;
	};
}