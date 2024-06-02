#pragma once
#include "ScriptDefines.h"
#include "Script.h"
#include "Contexts/ExecutionContextBase.h"

namespace SCR
{

	class ScriptFoundation;

	class ScriptManager final
	{
		friend class ScriptFoundation;
		friend class ScriptProxy;

		ScriptManager(ScriptFoundation& aFoundation);
	public:

		~ScriptManager();

		ScriptManager(const ScriptManager&) = delete;
		ScriptManager(ScriptManager&&) = delete;
		ScriptManager& operator=(const ScriptManager&) = delete;
		ScriptManager& operator=(ScriptManager&&) = delete;

		void TriggerEvent(const eNodeExecutionTrait anExecutionTrait, const ExecutionContextBase& aContext);

		Script& CreateScript(const std::string& aName = "Default Script");

		const std::vector<std::unique_ptr<Script>>& GetScripts();
		Script* GetScriptByName(const std::string& aName);

		void Clear();

	private:

		std::vector<std::unique_ptr<Script>> myScripts;

		ScriptFoundation& myFoundation;
	};
}