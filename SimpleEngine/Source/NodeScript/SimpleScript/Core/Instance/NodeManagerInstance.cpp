#include "NodeManagerInstance.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../DataType/DataTypeManager.hpp"
#include "../Node/NodeTypeManager.hpp"

namespace FLY_NAMESPACE
{

	NodeManagerInstance::NodeManagerInstance()
	{
		
	}

	NodeManagerInstance::~NodeManagerInstance()
	{
	}

	void NodeManagerInstance::Init(const std::vector<Node>& aNodeVector)
	{
		mNodeStateMap.clear();
		mMemoryArena.Clear();
		for (NodeID nodeID = 0; nodeID < aNodeVector.size(); ++nodeID)
		{
			const Node& node = aNodeVector[nodeID];
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
