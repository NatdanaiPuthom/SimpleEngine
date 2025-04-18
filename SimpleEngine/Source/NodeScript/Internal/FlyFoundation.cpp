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

	MemoryPool& Foundation::GetMemoryPool()
	{
		return mMemoryPool;
	}

	const MemoryPool& Foundation::GetMemoryPool() const
	{
		return mMemoryPool;
	}

	NodeTypeManager& Foundation::GetNodeTypeManager()
	{
		return mNodeTypeManager;
	}

	const NodeTypeManager& Foundation::GetNodeTypeManager() const
	{
		return mNodeTypeManager;
	}

	PinTypeManager& Foundation::GetPinTypeManager()
	{
		return mPinTypeManager;
	}

	const PinTypeManager& Foundation::GetPinTypeManager() const
	{
		return mPinTypeManager;
	}

	DataTypeManager& Foundation::GetDataTypeManager()
	{
		return mDataTypeManager;
	}

	const DataTypeManager& Foundation::GetDataTypeManager() const
	{
		return mDataTypeManager;
	}

	TraitManager& Foundation::GetTraitManager()
	{
		return mTraitManager;
	}

	const TraitManager& Foundation::GetTraitManager() const
	{
		return mTraitManager;
	}

	NodeExecutor& Foundation::GetNodeExecutor()
	{
		return *mNodeExecutor;
	}

	const NodeExecutor& Foundation::GetNodeExecutor() const
	{
		return *mNodeExecutor;
	}

	EventGraph& Foundation::GetNodeGraphCopy()
	{
		return mNodeGraphCopy;
	}

	const EventGraph& Foundation::GetNodeGraphCopy() const
	{
		return mNodeGraphCopy;
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