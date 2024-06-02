#include "VariableManager.h"
#include "../Script.h"
#include "ScriptMemoryPool.h"

namespace SCR
{

	VariableManager::VariableManager()
		: myMemoryPool(std::make_unique<MemoryPool>(100))
	{
	}

	VariableManager::~VariableManager()
	{
	}

	VarID VariableManager::GetVariableIDByNodeID(const NodeID aNodeID) const
	{
		return myNodeIDToVarID.at(aNodeID);
	}

	std::vector<NodeID> VariableManager::GetNodeIDsByVarID(const VarID aVarID) const
	{
		std::vector<NodeID> nodeIDs;
		for (auto& [nodeID, varID] : myNodeIDToVarID)
		{
			if (varID == aVarID)
			{
				nodeIDs.push_back(nodeID);
			}
		}
		return nodeIDs;
	}
}

