#include "Script.h"
#include "Utilities/ScriptProxy.h"
#include "Node/NodeExecutor.h"
#include "Command/ScriptCommandTracker.h"
#include "ScriptUtilities.h"
#include "Instance/ScriptInstance.h"
#include "Function/ScriptFunction.h"

namespace SCR
{

	Script::Script(const DataTypeID aTargetID, const std::string& aName)
		: myTargetID(aTargetID)
		, myName(aName)
	{
	}

	Script::~Script()
	{
	}

	std::string& Script::Name()
	{
		return myName;
	}

	const std::string& Script::Name() const
	{
		return myName;
	}

	ScriptInstance& Script::CreateScriptInstance()
	{
		std::unique_ptr<ScriptInstance>& scriptInstance = myScriptInstances.emplace_back(std::make_unique<ScriptInstance>());
		scriptInstance->Init(*this);
		
		return *scriptInstance;
	}

	void Script::DestroyScriptInstance(ScriptInstance& aScriptInstance)
	{
		std::erase_if(myScriptInstances,
			[&aScriptInstance](const std::unique_ptr<ScriptInstance>& scriptInstanceIter) -> bool
			{
				return &aScriptInstance == scriptInstanceIter.get();
			}
		);
	}

	EventGraph& Script::GetEventGraph()
	{
		return myEventGraph;
	}

	DataTypeID Script::GetTargetID() const
	{
		return myTargetID;
	}

	void Script::BindFunction(FunctionID aFunctionID)
	{
		myMemberFunctionIDs.push_back(aFunctionID);
	}

	const std::vector<FunctionID>& Script::GetMemberFunctionIDs() const
	{
		return myMemberFunctionIDs;
	}

	const VariableManager& Script::GetVariableManager() const
	{
		return myVariableManager;
	}
}

