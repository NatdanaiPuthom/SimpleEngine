#pragma once
#include "../ScriptDefines.h"
#include "Node.h"
#include "NodeType.h"
#include "../DataStructures/ScriptTypeContainer.h"
#include <unordered_map>
#include <any>

namespace SCR
{

	class Script;

	class NodeManager
	{
		friend class ScriptProxy;
	public:

		NodeManager();
		~NodeManager();

		NodeManager(const NodeManager&);

		TypeContainer<NodeID, const Node, const NodeType>::Iterator begin() const;
		TypeContainer<NodeID, const Node, const NodeType>::Iterator end() const;

		TypeContainer<NodeID, Node, NodeType>::Iterator begin();
		TypeContainer<NodeID, Node, NodeType>::Iterator end();

	public:

		std::vector<Node> myNodes;

	};

}