#include "VariableManager.hpp"
#include "ScriptMemoryArena.hpp"

namespace SCR
{

	VariableManager::VariableManager()
		: mMemoryArena(std::make_unique<MemoryArena<NodeBufferCapacity>>())
	{
	}

	VariableManager::~VariableManager()
	{
	}

	VarID VariableManager::GetVariableIDByNodeRef(const NodeRef& aNodeRef) const
	{
		return mNodeRefToVarID.at(aNodeRef);
	}

	std::vector<NodeRef> VariableManager::GetNodeRefsByVarID(const VarID aVarID) const
	{
		std::vector<NodeRef> nodeIDs;
		for (auto& [nodeRef, varID] : mNodeRefToVarID)
		{
			if (varID == aVarID)
			{
				nodeIDs.push_back(nodeRef);
			}
		}
		return nodeIDs;
	}
}

