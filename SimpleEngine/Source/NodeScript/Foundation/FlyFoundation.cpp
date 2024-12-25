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
	{
	}

	Foundation::~Foundation()
	{
	}

	void Foundation::Initialize()
	{
		mTypeManager.GetNodeTypeManager().Assert();

		Internal::InitializeSubPins();
	}

	void Foundation::ClearClasses()
	{
		mClasses.clear();
	}

	StructID Foundation::CreateStruct(const std::string_view aName)
	{
		StructID id{ mStructs.size() };
		mStructs.emplace_back(HeapObject<Struct>(aName));
		return id;
	}

	ClassID Foundation::CreateClass(const DataTypeID aTargetID, const std::string_view aName)
	{
		ClassID id{ mClasses.size() };
		mClasses.emplace_back(HeapObject<Class>(aTargetID, std::string(aName)));
		return id;
	}

	void Foundation::DestroyClass(Class& aClass)
	{
		std::erase_if(mClasses, [&aClass](HeapObject<Class>& aClassIter) -> bool { return &aClass == &*aClassIter; });
	}

	Struct& Foundation::GetStruct(const StructID aID)
	{
		return *mStructs[aID];
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