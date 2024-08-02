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
		, mTypeManager(std::make_unique<TypeManager>())
		, mNodeExecutor(std::make_unique<NodeExecutor>())
	{
	}

	Foundation::~Foundation()
	{
	}

	void Foundation::Initialize()
	{
		mTypeManager->GetNodeTypeManager().Assert();
	}

	void Foundation::ClearClasses()
	{
		mClasses.clear();
	}

	Class& Foundation::CreateClass(const DataTypeID aTargetID, const std::string_view aName)
	{
		std::unique_ptr<Class>& createdClass = mClasses.emplace_back(std::make_unique<Class>(aTargetID, std::string(aName)));
		return *createdClass;
	}

	void Foundation::DestroyClass(Class& aClass)
	{
		std::erase_if(mClasses, [&aClass](std::unique_ptr<Class>& aClassIter) -> bool { return &aClass == aClassIter.get(); });
	}

	Class& Foundation::GetClassByID(const ClassID aID)
	{
		return *mClasses.at(aID);
	}

	const std::vector<std::unique_ptr<Class>>& Foundation::GetClasses() const
	{
		return mClasses;
	}

	TypeManager& Foundation::GetTypeManager()
	{
		return *mTypeManager;
	}

	NodeExecutor& Foundation::GetNodeExecutor()
	{
		return *mNodeExecutor;
	}


	const VariableRef& Foundation::GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef) const
	{
		return mNodeRefToVarRef.at(aNodeRef);
	}

	/*VarID Foundation::GetVariableIDByNodeRef(const GlobalNodeRefConst& aNodeRef) const
	{
		return GetVariableIDByNodeRef(*reinterpret_cast<const GlobalNodeRef*>(&aNodeRef));
	}*/

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