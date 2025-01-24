#include "FlyFoundation.hpp"
#include "NodeTypes/SystemNodes.hpp"

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
		mNodeExecutor->Initialize();
		mNodeTypeManager.Assert();

		Internal::InitializeSubPins();

		TestUtilityFunction();
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

	FLY_VALUETYPE(Wildcard, Colors::Gray);
	FLY_VALUETYPE(Flow, eNodeOperatorTrait::None, Color(0.9f, 0.9f, 0.9f));
	FLY_POINTERTYPE(None, Colors::Black);
}