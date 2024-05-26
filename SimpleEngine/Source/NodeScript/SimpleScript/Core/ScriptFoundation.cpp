#include "ScriptFoundation.h"
#include "ScriptManager.h"
#include "NodeTypes/SystemNodes.h"
#include "Serialization/ScriptLoader.h"
#include "SystemDataTypes.h"

namespace SCR
{

	MemoryPool ScriptFoundation::myGlobalMemoryPool = MemoryPool(10000);

	ScriptFoundation::ScriptFoundation()
	{
	}

	ScriptFoundation::~ScriptFoundation()
	{
		NodeTypeManager::Destroy();
		PinTypeManager::Destroy();
		DataTypeManager::Destroy();

		myGlobalMemoryPool.Clear();
		myGlobalMemoryPool.~MemoryPool();
	}

	void ScriptFoundation::InitializeSystemTypes()
	{
		RegisterSystemDataTypes();
		RegisterSystemNodes();

		ScriptLoader::LoadCustomEvents(*this);
		NodeTypeManager::Assert();
	}

	ScriptManager& ScriptFoundation::CreateScriptManager()
	{
		ScriptManager* scriptManager = new ScriptManager(*this);
		myScriptManagers.push_back(std::unique_ptr<ScriptManager>(scriptManager));
		return *myScriptManagers.back();
	}

	void ScriptFoundation::DestroyScriptManager(ScriptManager& aScriptManager)
	{
		for (size_t i = 0; i < myScriptManagers.size(); i++)
		{
			if (myScriptManagers[i].get() == &aScriptManager)
			{
				myScriptManagers.erase(myScriptManagers.begin() + i);
				
				return;
			}
		}
	}

	void ScriptFoundation::Clear()
	{
		myScriptManagers.clear();
	}
}