#pragma once
#include "ScriptDefines.h"
#include "Variable/VariableManager.h"
#include "Node/NodeManager.h"
#include "Pin/PinManager.h"
#include "Node/NodeExecutor.h"
#include "Utilities/ScriptLinker.h"
#include "ScriptModifier.h"
#include "Command/ScriptCommandTracker.h"
#include "Memory/ScriptMemoryPool.h"
#include "ScriptInternalModifier.h"

namespace SCR
{

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

		void TriggerEvent(const std::string& aUserEventKey, const ExecutionContextBase& anExecutionContext);
		void TriggerEvent(const eNodeExecutionTrait anExecutionTrait, const ExecutionContextBase& anExecutionContext);

		std::string& Name();
		const std::string& Name() const;

		ScriptModifier& GetModifier();
		CommandTracker& GetCommandTracker();

	private:

		ScriptInternalModifier& GetInternalModifier();

	private:

		std::string myName;

		MemoryPool myMemoryPool;

		NodeManager myNodeManager;
		PinManager myPinManager;

		VariableManager myVariableManager;
		
		ScriptInternalModifier myInternalModifier;

		NodeExecutor myExecutor;

		ScriptModifier myModifier;
		CommandTracker myCommandTracker;

		ScriptManager& myScriptManager;


	};
}

