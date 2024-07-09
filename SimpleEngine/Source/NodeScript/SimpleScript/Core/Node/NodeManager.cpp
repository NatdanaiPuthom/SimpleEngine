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
	{
	}

	TypeContainer<NodeID, const Node, const NodeType>::Iterator NodeManager::begin() const
	{
		auto f = [](const Node& aNode) -> const NodeType& { return NodeTypeManager::GetInstance().GetNodeType(aNode.typeID); };
		return TypeContainer<NodeID, const Node, const NodeType>(myNodes, f).begin();
	}

	TypeContainer<NodeID, const Node, const NodeType>::Iterator NodeManager::end() const
	{
		auto f = [](const Node& aNode) -> const NodeType& { return NodeTypeManager::GetInstance().GetNodeType(aNode.typeID); };
		return TypeContainer<NodeID, const Node, const NodeType>(myNodes, f).end();
	}

	TypeContainer<NodeID, Node, NodeType>::Iterator NodeManager::begin()
	{
		auto f = [](const Node& aNode) -> NodeType& { return NodeTypeManager::GetInstance().GetNodeType(aNode.typeID); };
		return TypeContainer<NodeID, Node, NodeType>(myNodes, f).begin();
	}

	TypeContainer<NodeID, Node, NodeType>::Iterator NodeManager::end()
	{
		auto f = [](const Node& aNode) -> NodeType& { return NodeTypeManager::GetInstance().GetNodeType(aNode.typeID); };
		return TypeContainer<NodeID, Node, NodeType>(myNodes, f).end();
	}
}
