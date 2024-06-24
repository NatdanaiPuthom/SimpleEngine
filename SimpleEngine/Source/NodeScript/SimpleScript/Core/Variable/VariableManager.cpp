#include "VariableManager.h"
#include "../Script.h"
#include "ScriptMemoryPool.h"
#include "ScriptMemoryManager.h"

namespace SCR
{

	VariableManager::VariableManager()
		: myMemoryManager(std::make_unique<MemoryManager>())
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

