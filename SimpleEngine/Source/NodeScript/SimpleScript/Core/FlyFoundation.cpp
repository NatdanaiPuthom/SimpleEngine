#include "FlyFoundation.hpp"
#include "FlyClass.hpp"
#include "NodeTypes/SystemNodes.hpp"
#include "FlyDataTypes.hpp"
#include "Serialization/FlySerializer.hpp"
#include "Node/FlyNodeExecutor.hpp"
#include "Type/FlyTypeManager.hpp"

namespace FLY_NAMESPACE
{
	void ScriptFoundation::Destroy()
	{
		if (sInstance)
		{
			delete sInstance;
			sInstance = nullptr;
		}
	}
	ScriptFoundation::ScriptFoundation()
		: mMemoryPool(10000)
		, mTypeManager(std::make_unique<TypeManager>())
		, mNodeExecutor(std::make_unique<NodeExecutor>())
	{
	}

	ScriptFoundation::~ScriptFoundation()
	{
	}

	void ScriptFoundation::Initialize()
	{
		mTypeManager->GetNodeTypeManager().Assert();
	}

	void ScriptFoundation::ClearClasses()
	{
		mClasses.clear();
	}

	Class& ScriptFoundation::CreateClass(DataTypeID aTargetID, const std::string_view aName)
	{
		std::vector<std::unique_ptr<Class>>& classesByTarget = mClasses[aTargetID];
		return *classesByTarget.emplace_back(std::make_unique<Class>(aTargetID, std::string(aName)));
	}

	void ScriptFoundation::DestroyClass(Class& aClass)
	{
		auto& scriptsByTargetID = mClasses.at(aClass.GetTargetID());

		std::erase_if(scriptsByTargetID, [&aClass](std::unique_ptr<Class>& aClassIter) -> bool { return &aClass == aClassIter.get(); });
	}

	const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>>& ScriptFoundation::GetClasses()
	{
		return mClasses;
	}

	TypeManager& ScriptFoundation::GetTypeManager()
	{
		return *mTypeManager;
	}

	NodeExecutor& ScriptFoundation::GetNodeExecutor()
	{
		return *mNodeExecutor;
	}
}