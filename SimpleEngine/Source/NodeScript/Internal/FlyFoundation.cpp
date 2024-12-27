#include "FlyFoundation.hpp"
#include "FlyClass.hpp"
#include "NodeTypes/SystemNodes.hpp"
#include "FlyDataTypes.hpp"
#include "Serialization/FlySerializer.hpp"
#include "Execution/FlyNodeExecutor.hpp"

namespace FLY_NAMESPACE
{
	void Foundation::Destroy()
	{
		if (sInstance)
		{
			delete sInstance;
			sInstance = nullptr;
		}
	}

	Foundation::Foundation()
		: mMemoryPool(10000)
	{
	}

	Foundation::~Foundation()
	{
	}

	void Foundation::Initialize()
	{
		mNodeTypeManager.Assert();

		Internal::InitializeSubPins();
	}

	NodeExecutor& Foundation::GetNodeExecutor()
	{
		return *mNodeExecutor;
	}

	const VariableRef& Foundation::GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef) const
	{
		return mNodeRefToVarRef.at(aNodeRef);
	}

	std::vector<GlobalNodeRef> Foundation::GetNodeRefsByVariableRef(const VariableRef& aVarRef) const
	{
		std::vector<GlobalNodeRef> nodeIDs;
		for (auto& [nodeRef, varID] : mNodeRefToVarRef)
		{
			if (varID == aVarRef)
			{
				nodeIDs.push_back(nodeRef);
			}
		}
		return nodeIDs;
	}
}