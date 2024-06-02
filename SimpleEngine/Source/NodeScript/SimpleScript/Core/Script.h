#pragma once
#include "ScriptDefines.h"
#include "Variable/VariableManager.h"
#include "ScriptModifier.h"
#include "ScriptNodeGraph.h"

namespace SCR
{

	class CommandTracker;
	class ScriptManager;
	struct ExecutionContextBase;

	class Script final
	{
		friend class ScriptProxy;
	public:

		Script(ScriptManager& aScriptManager, const std::string& aName = "Default Script");
		~Script();

		Script(const Script&) = delete;
		Script(Script&&) = delete;
		Script& operator=(const Script&) = delete;
		Script& operator=(Script&&) = delete;

		void TriggerEvent(const eNodeExecutionTrait anExecutionTrait, const ExecutionContextBase& anExecutionContext);

		std::string& Name();
		const std::string& Name() const;

		ScriptModifier& GetModifier();
		CommandTracker& GetCommandTracker();
		NodeGraph& GetEventGraph();


	private:

		std::string myName;
		NodeGraph myEventGraph;

		VariableManager myVariableManager;

		//NodeExecutor myExecutor;

		ScriptModifier myModifier;
		std::unique_ptr<CommandTracker> myCommandTracker;

		ScriptManager& myScriptManager;


	};
}

