#include "FlyNodeManagerInstance.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Node/FlyNodeTypeManager.hpp"

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
			void* dataPtr = Global::GetDataTypeManager().AllocateData(nodeType.mNodeRecipe.mNodeStateDataTypeID, mMemoryArena);
			assert(dataPtr != nullptr);
			mNodeStateMap.emplace(nodeID, dataPtr);
		}
	}
}
