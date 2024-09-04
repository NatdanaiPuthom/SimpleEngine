#include "FlyNodeGraphInstance.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{
	NodeGraphInstance::NodeGraphInstance(const NodeGraph& aNodeGraph)
		: mNodeGraph(&aNodeGraph)
	{
		for (NodeID nodeID = 0; nodeID < aNodeGraph.mNodes.size(); ++nodeID)
		{
			const Node& node = aNodeGraph.mNodes[nodeID];
			const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);
			if (nodeType.mNodeRecipe.mNodeStateDataTypeID == InvalidID<DataTypeID>())
			{
				continue;
			}
			void* const dataPtr = Global::GetDataTypeManager().AllocateData(nodeType.mNodeRecipe.mNodeStateDataTypeID, mMemoryArena);
			assert(dataPtr != nullptr);
			mNodeStateMap.emplace(nodeID, dataPtr);
		}
	}

	NodeGraphInstance::~NodeGraphInstance()
	{
	}

	void NodeGraphInstance::Mirror()
	{
		for (NodeID nodeID = 0; nodeID < mNodeGraph->mNodes.size(); ++nodeID)
		{
			const Node& node = mNodeGraph->mNodes[nodeID];
			const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);
			if (nodeType.mNodeRecipe.mNodeStateDataTypeID == InvalidID<DataTypeID>())
			{
				continue;
			}
			if (mNodeStateMap.contains(nodeID))
			{
				continue;
			}
			void* const dataPtr = Global::GetDataTypeManager().AllocateData(nodeType.mNodeRecipe.mNodeStateDataTypeID, mMemoryArena);
			assert(dataPtr != nullptr);
			mNodeStateMap.emplace(nodeID, dataPtr);
		}
	}
}

