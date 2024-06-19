#include "Script.h"
#include "Utilities/ScriptProxy.h"
#include "Node/NodeExecutor.h"
#include "Command/ScriptCommandTracker.h"
#include "ScriptUtilities.h"
#include "Instance/ScriptInstance.h"

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
		for (size_t i = 0; i < myScriptInstances.size(); ++i)
		{
			if (myScriptInstances[i].get() == &aScriptInstance)
			{
				myScriptInstances.erase(myScriptInstances.begin() + i);
			}
		}
	}

	EventGraph& Script::GetEventGraph()
	{
		return myEventGraph;
	}
}

