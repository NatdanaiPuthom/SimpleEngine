#include "NodeManagerInstance.hpp"
#include "../Node/NodeManager.hpp"
#include "../Global/ScriptGlobal.hpp"
#include "../DataType/DataTypeManager.hpp"
#include "../Node/NodeTypeManager.hpp"

namespace SCR
{

	NodeManagerInstance::NodeManagerInstance()
	{
		
	}

	NodeManagerInstance::~NodeManagerInstance()
	{
	}

	void NodeManagerInstance::Init(const NodeManager& aNodeManager)
	{
		mNodeStateMap.clear();
		mMemoryArena.Clear();
		for (NodeID nodeID = 0; nodeID < aNodeManager.mNodes.size(); ++nodeID)
		{
			const Node& node = aNodeManager.mNodes[nodeID];
			const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);
			if (nodeType.mNodeRecipe.mNodeStateDataTypeID == InvalidID<DataTypeID>())
			{
				continue;
			}
			void* mDataPtr = Global::GetDataTypeManager().AllocateData(nodeType.mNodeRecipe.mNodeStateDataTypeID, mMemoryArena);
			assert(mDataPtr != nullptr);
			mNodeStateMap.emplace(nodeID, mDataPtr);
		}
	}
}
