#pragma once
#include "ScriptDefines.hpp"
#include "Node/NodeManager.hpp"
#include "Pin/PinManager.hpp"
#include "Memory/ScriptMemoryArena.hpp"

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

		std::unique_ptr<NodeManager> mNodeManager;
		std::unique_ptr<PinManager> mPinManager;
		std::vector<Link> mLinks;

		MemoryArena<NodeBufferCapacity> mMemoryArena;

	private:

		eNodeGraphType mType;
	};
}