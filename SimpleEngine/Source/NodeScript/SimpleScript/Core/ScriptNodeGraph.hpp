#pragma once
#include "ScriptDefines.hpp"
#include "Node/Node.hpp"
#include "Pin/Pin.hpp"
#include "Memory/ScriptMemoryArena.hpp"

namespace FLY_NAMESPACE
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


		std::vector<Node> mNodes;
		std::vector<Pin> mPins;
		std::vector<Link> mLinks;

		MemoryArena<NodeBufferCapacity> mMemoryArena;

	private:

		eNodeGraphType mType;
	};
}