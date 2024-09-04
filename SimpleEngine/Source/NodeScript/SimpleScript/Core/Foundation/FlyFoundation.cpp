#include "FlyFoundation.hpp"
#include "FlyClass.hpp"
#include "NodeTypes/SystemNodes.hpp"
#include "FlyDataTypes.hpp"
#include "Serialization/FlySerializer.hpp"
#include "Execution/FlyNodeExecutor.hpp"
#include "Type/FlyTypeManager.hpp"

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
		, mNodeExecutor(MakeHeapObject<NodeExecutor>())
	{
	}

	Foundation::~Foundation()
	{
	}

	void Foundation::Initialize()
	{
		mTypeManager.GetNodeTypeManager().Assert();
	}

	void Foundation::ClearClasses()
	{
		mClasses.clear();
	}

	Class& Foundation::CreateClass(const DataTypeID aTargetID, const std::string_view aName)
	{
		return *mClasses.emplace_back(MakeHeapObject<Class>(aTargetID, std::string(aName)));
	}

	void Foundation::DestroyClass(Class& aClass)
	{
		std::erase_if(mClasses, [&aClass](HeapObject<Class>& aClassIter) -> bool { return &aClass == aClassIter.Get(); });
	}

	const std::vector<HeapObject<Class>>& Foundation::GetClasses() const
	{
		return mClasses;
	}

	TypeManager& Foundation::GetTypeManager()
	{
		return mTypeManager;
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