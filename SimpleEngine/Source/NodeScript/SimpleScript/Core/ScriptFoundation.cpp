#include "ScriptFoundation.hpp"
#include "FlyClass.hpp"
#include "NodeTypes/SystemNodes.hpp"
#include "SystemDataTypes.hpp"
#include "Serialization/ScriptLoader.hpp"
#include "Node/NodeExecutor.hpp"
#include "Type/ScriptTypeManager.hpp"

namespace FLY_NAMESPACE
{
	void ScriptFoundation::Destroy()
	{
		if (myInstance)
		{
			delete myInstance;
			myInstance = nullptr;
		}
	}
	ScriptFoundation::ScriptFoundation()
		: myMemoryPool(10000)
		, myTypeManager(std::make_unique<TypeManager>())
		, myNodeExecutor(std::make_unique<NodeExecutor>())
	{
	}

	ScriptFoundation::~ScriptFoundation()
	{
	}

	void ScriptFoundation::Initialize()
	{
		myTypeManager->GetNodeTypeManager().Assert();
	}

	void ScriptFoundation::ClearScripts()
	{
		myScripts.clear();
	}

	Class& ScriptFoundation::CreateClass(DataTypeID aTargetID, const std::string& aName)
	{
		std::vector<std::unique_ptr<Class>>& classesByTarget = myScripts[aTargetID];
		return *classesByTarget.emplace_back(std::make_unique<Class>(aTargetID, aName));
	}

	void ScriptFoundation::DestroyScript(Class& aScript)
	{
		auto& scriptsByTargetID = myScripts.at(aScript.GetTargetID());

		std::erase_if(scriptsByTargetID, [&aScript](std::unique_ptr<Class>& scriptIter) -> bool { return &aScript == scriptIter.get(); });
	}

	const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>>& ScriptFoundation::GetScripts()
	{
		return myScripts;
	}

	TypeManager& ScriptFoundation::GetTypeManager()
	{
		return *myTypeManager;
	}

	NodeExecutor& ScriptFoundation::GetNodeExecutor()
	{
		return *myNodeExecutor;
	}
}