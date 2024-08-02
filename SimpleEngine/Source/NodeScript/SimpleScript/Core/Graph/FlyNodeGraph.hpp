#pragma once
#include "FlyDefines.hpp"
#include "Node/FlyNode.hpp"
#include "Pin/FlyPin.hpp"
#include "Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	class NodeGraph final
	{
	public:

		NodeGraph() = default;
		~NodeGraph();

		NodeGraph(const NodeGraph&);
		NodeGraph(NodeGraph&&) = default;

		NodeGraph& operator=(const NodeGraph&) = delete;
		NodeGraph& operator=(NodeGraph&&) = default;

	public:


		std::vector<Node> mNodes;
		std::vector<Pin> mPins;
		std::vector<Link> mLinks;

		MemoryArena<NodeBufferCapacity> mMemoryArena;
	};
}