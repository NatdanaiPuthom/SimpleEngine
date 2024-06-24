#include "NodeManagerInstance.h"
#include "../Node/NodeManager.h"
#include "../Memory/ScriptMemoryManager.h"
#include "../Global/ScriptGlobal.h"
#include "../DataType/DataTypeManager.h"

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
		myNodeStateMap.clear();
		myMemoryManager.Clear();
		for (auto& [nodeID, node, nodeType] : aNodeManager)
		{
			if (nodeType->nodeRecipe.nodeStateDataTypeID == InvalidID<DataTypeID>())
			{
				continue;
			}
			void* dataPtr = Global::GetDataTypeManager().AllocateData(nodeType->nodeRecipe.nodeStateDataTypeID, myMemoryManager.GetMemory());
			assert(dataPtr != nullptr);
			myNodeStateMap.emplace(nodeID, dataPtr);
		}
	}
}
