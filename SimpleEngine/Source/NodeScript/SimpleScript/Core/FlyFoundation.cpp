#include "FlyFoundation.hpp"
#include "FlyClass.hpp"
#include "NodeTypes/SystemNodes.hpp"
#include "FlyDataTypes.hpp"
#include "Serialization/FlySerializer.hpp"
#include "Node/FlyNodeExecutor.hpp"
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

	Class& Foundation::CreateClass(DataTypeID aTargetID, const std::string_view aName)
	{
		std::vector<std::unique_ptr<Class>>& classesByTarget = mClasses[aTargetID];
		
		std::unique_ptr<Class>& createdClass = classesByTarget.emplace_back(std::make_unique<Class>(aTargetID, std::string(aName)));
		mClassesByName.emplace(createdClass->mName, createdClass.get());
		return *createdClass;
	}

	void Foundation::DestroyClass(Class& aClass)
	{
		auto& scriptsByTargetID = mClasses.at(aClass.mTargetID);
		mClassesByName.erase(aClass.mName);

		std::erase_if(scriptsByTargetID, [&aClass](std::unique_ptr<Class>& aClassIter) -> bool { return &aClass == aClassIter.get(); });
	}

	void Foundation::SetClassName(std::string_view aOldName, std::string_view aNewName)
	{
		if (aOldName == aNewName)
		{
			return;
		}

		Class* c = mClassesByName.at(aOldName);
		c->mName = aNewName;
		mClassesByName.erase(aOldName);
		mClassesByName.emplace(c->mName, c);
	}

	const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>>& Foundation::GetClasses()
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
}