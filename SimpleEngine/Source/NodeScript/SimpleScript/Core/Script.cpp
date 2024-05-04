#include "Script.h"
#include "ScriptManager.h"

namespace SCR
{

	Script::Script(ScriptManager& aScriptManager, const std::string& aName)
		: myName(aName)
		, myInternalModifier(*this)
		, myExecutor(myInternalModifier)
		, myModifier(*this)
		, myCommandTracker(*this)
		, myMemoryPool(200)
		, myScriptManager(aScriptManager)
	{
		myInternalModifier.UpdateNodeTypeIDSize();
	}

	Script::~Script()
	{
	}

	void Script::TriggerEvent(const std::string& aUserEventKey, const ExecutionContextBase& anExecutionContext)
	{
		myExecutor.Execute(aUserEventKey, anExecutionContext);
	}

	void Script::TriggerEvent(const eNodeExecutionTrait anExecutionTrait, const ExecutionContextBase& anExecutionContext)
	{
		myExecutor.Execute(anExecutionTrait, anExecutionContext);
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
		return myCommandTracker;
	}

	ScriptInternalModifier& Script::GetInternalModifier()
	{
		return myInternalModifier;
	}
}

