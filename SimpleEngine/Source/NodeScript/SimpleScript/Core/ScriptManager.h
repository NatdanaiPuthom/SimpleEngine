#pragma once
#include "ScriptDefines.h"
#include "Script.h"
#include "Contexts/ExecutionContextBase.h"

namespace SCR
{

	class ScriptFoundation;

	class ScriptManager final
	{
		friend class ScriptProxy;

	public:

		ScriptManager(ScriptFoundation& aFoundation);
		~ScriptManager();

		ScriptManager(const ScriptManager&) = delete;
		ScriptManager(ScriptManager&&) = delete;
		ScriptManager& operator=(const ScriptManager&) = delete;
		ScriptManager& operator=(ScriptManager&&) = delete;

		Script& CreateScript(const std::string& aName = "Default Script");

		const std::vector<std::unique_ptr<Script>>& GetScripts();
		Script* GetScriptByName(const std::string& aName);

		void Clear();

	private:

		std::vector<std::unique_ptr<Script>> myScripts;

		ScriptFoundation& myFoundation;
	};
}