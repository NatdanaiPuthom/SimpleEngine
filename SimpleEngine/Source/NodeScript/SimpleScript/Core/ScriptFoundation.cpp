#include "ScriptFoundation.h"
#include "Script.h"
#include "NodeTypes/SystemNodes.h"
#include "SystemDataTypes.h"
#include "Serialization/ScriptLoader.h"
#include "Node/NodeExecutor.h"
#include "Type/ScriptTypeManager.h"

namespace SCR
{

	ScriptFoundation::ScriptFoundation()
		: myMemoryPool(10000)
		, myTypeManager(std::make_unique<TypeManager>())
		, myNodeExecutor(std::make_unique<NodeExecutor>())
	{
	}

	ScriptFoundation::~ScriptFoundation()
	{
	}

	void ScriptFoundation::InitializeSystemTypes()
	{

		ScriptLoader::LoadCustomEvents();
		myTypeManager->GetNodeTypeManager().Assert();
	}

	void ScriptFoundation::ClearScripts()
	{
		myScripts.clear();
	}

	Script& ScriptFoundation::CreateScript(DataTypeID aTargetID, const std::string& aName)
	{
		std::vector<std::unique_ptr<Script>>& scriptsByTarget = myScripts[aTargetID];
		return *scriptsByTarget.emplace_back(std::make_unique<Script>(aTargetID, aName));
	}

	void ScriptFoundation::DestroyScript(Script& aScript)
	{
		auto& scriptsByTargetID = myScripts.at(aScript.GetTargetID());

		std::erase_if(scriptsByTargetID, [&aScript](std::unique_ptr<Script>& scriptIter) -> bool { return &aScript == scriptIter.get(); });
	}

	const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Script>>>& ScriptFoundation::GetScripts()
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