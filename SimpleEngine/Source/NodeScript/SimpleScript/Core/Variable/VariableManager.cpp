#include "VariableManager.h"
#include "ScriptMemoryArena.h"

namespace SCR
{

	VariableManager::VariableManager()
		: myMemoryArena(std::make_unique<MemoryArena<NodeBufferCapacity>>())
	{
	}

	VariableManager::~VariableManager()
	{
	}

	VarID VariableManager::GetVariableIDByNodeRef(const NodeRef& aNodeRef) const
	{
		return myNodeRefToVarID.at(aNodeRef);
	}

	std::vector<NodeRef> VariableManager::GetNodeRefsByVarID(const VarID aVarID) const
	{
		std::vector<NodeRef> nodeIDs;
		for (auto& [nodeRef, varID] : myNodeRefToVarID)
		{
			if (varID == aVarID)
			{
				nodeIDs.push_back(nodeRef);
			}
		}
		return nodeIDs;
	}
}

