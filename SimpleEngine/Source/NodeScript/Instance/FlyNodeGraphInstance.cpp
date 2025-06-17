#include "FlyNodeGraphInstance.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{
	NodeGraphInstance::NodeGraphInstance(const NodeGraph& aNodeGraph)
		: mNodeGraph(&aNodeGraph)
	{
		for (NodeID nodeID{ 0 }; nodeID < aNodeGraph.GetNodes().size(); ++nodeID)
		{
			const Node& node = aNodeGraph.GetNode(nodeID);
			const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.GetTypeID());
			if (nodeType.GetNodeStateDataTypeID() == InvalidID<DataTypeID>())
			{
				continue;
			}
			void* const dataPtr = Internal::GetDataTypeManager().AllocateData(nodeType.GetNodeStateDataTypeID(), mMemoryArena);
			assert(dataPtr != nullptr);
			mNodeStateMap.emplace(nodeID, dataPtr);
		}
	}

	NodeGraphInstance::~NodeGraphInstance() = default;

	void NodeGraphInstance::Mirror()
	{
		for (NodeID nodeID{ 0 }; nodeID < mNodeGraph->GetNodeCount(); ++nodeID)
		{
			const Node& node = mNodeGraph->GetNode(nodeID);
			const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.GetTypeID());
			if (nodeType.GetNodeStateDataTypeID() == InvalidID<DataTypeID>())
			{
				continue;
			}
			if (mNodeStateMap.contains(nodeID))
			{
				continue;
			}
			void* const dataPtr = Internal::GetDataTypeManager().AllocateData(nodeType.GetNodeStateDataTypeID(), mMemoryArena);
			assert(dataPtr != nullptr);
			mNodeStateMap.emplace(nodeID, dataPtr);
		}
	}
}

