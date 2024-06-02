#include "Script.h"
#include "Utilities/ScriptProxy.h"
#include "Node/NodeExecutor.h"
#include "Command/ScriptCommandTracker.h"

namespace SCR
{

	Script::Script(ScriptManager& aScriptManager, const std::string& aName)
		: myName(aName)
		, myModifier(*this)
		, myCommandTracker(std::make_unique<CommandTracker>())
		, myScriptManager(aScriptManager)
	{
	}

	Script::~Script()
	{
	}

	void Script::TriggerEvent(const eNodeExecutionTrait anExecutionTrait, const ExecutionContextBase& anExecutionContext)
	{
		ScriptProxy::GetNodeExecutor(*this).ExecuteEvent(anExecutionTrait, *this, anExecutionContext);
		//myExecutor.Execute(anExecutionTrait, anExecutionContext);
	}

	std::string& Script::Name()
	{
		return myName;
	}

	const std::string& Script::Name() const
	{
		return myName;
	}

	ScriptModifier& Script::GetModifier()
	{
		return myModifier;
	}

	CommandTracker& Script::GetCommandTracker()
	{
		return *myCommandTracker;
	}

	NodeGraph& Script::GetEventGraph()
	{
		return myEventGraph;
	}
}

