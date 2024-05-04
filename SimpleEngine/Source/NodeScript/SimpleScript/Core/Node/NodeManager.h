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

		TypeContainer<NodeID, Node, NodeType>::ConstIterator begin() const;
		TypeContainer<NodeID, Node, NodeType>::ConstIterator end() const;

	private:

		std::vector<Node> myNodes;
		std::vector<std::vector<NodeID>> myNodeIDsByNodeTypeID;

		std::unordered_map<NodeID, std::any> myInternalNodeData;

	};

}