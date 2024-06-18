#include "ScriptManager.h"
#include "Script.h"

namespace SCR
{

	ScriptManager::ScriptManager(ScriptFoundation& aFoundation)
		: myFoundation(aFoundation)
	{
	}

	ScriptManager::~ScriptManager()
	{
		myScripts.clear();
	}

	Script& ScriptManager::CreateScript(const std::string& aName)
	{
		myScripts.push_back(std::make_unique<Script>(*this, aName));
		return *myScripts.back();
	}

	const std::vector<std::unique_ptr<Script>>& ScriptManager::GetScripts()
	{
		return myScripts;
	}

	Script* ScriptManager::GetScriptByName(const std::string& aName)
	{
		for (size_t i = 0; i < myScripts.size(); i++)
		{
			if (myScripts[i]->Name() == aName)
			{
				return myScripts[i].get();
			}
		}
		return nullptr;
	}


	void ScriptManager::Clear()
	{
		myScripts.clear();
	}
}