#include "NodeManager.h"
#include "NodeTypeManager.h"

namespace SCR
{

	NodeManager::NodeManager()
	{
		myNodes.reserve(10);
	}

	NodeManager::~NodeManager()
	{
	}

	NodeManager::NodeManager(const NodeManager& aOther)
		: myNodes(aOther.myNodes)
		, myNodeIDsByNodeTypeID(aOther.myNodeIDsByNodeTypeID)
		, myInternalNodeData(aOther.myInternalNodeData)
	{
	}

	TypeContainer<NodeID, Node, NodeType>::ConstIterator NodeManager::begin() const
	{
		return TypeContainer<NodeID, Node, NodeType>(myNodes, [](const Node& aNode) -> const NodeType& { return NodeTypeManager::GetInstance().GetNodeType(aNode.typeID); }).begin();
	}

	TypeContainer<NodeID, Node, NodeType>::ConstIterator NodeManager::end() const
	{
		return TypeContainer<NodeID, Node, NodeType>(myNodes, [](const Node& aNode) -> const NodeType& { return NodeTypeManager::GetInstance().GetNodeType(aNode.typeID); }).end();
	}
}
