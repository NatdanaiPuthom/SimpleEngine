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

		TypeContainer<NodeID, Node, NodeType>::ConstIterator begin() const;
		TypeContainer<NodeID, Node, NodeType>::ConstIterator end() const;

	public:

		std::vector<Node> myNodes;
		std::vector<std::vector<NodeID>> myNodeIDsByNodeTypeID;

	};

}