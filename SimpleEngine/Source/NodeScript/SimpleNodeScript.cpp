#include "SimpleNodeScript.hpp"

namespace SCRIPT
{
	SimpleNodeScript::SimpleNodeScript()
		: myScriptManager(nullptr)
	{
	}

	SimpleNodeScript::~SimpleNodeScript()
	{
		myScriptManager = nullptr;
	}

	void SimpleNodeScript::Init()
	{
		myScriptFoundation.InitializeSystemTypes();
		RegisterSimpleGameNodes();

		myScriptManager = &myScriptFoundation.CreateScriptManager();

		myName = "world_Middle";
		SCRIPT::ScriptLoader::SavePath = "../Source/Script/data/SimpleScripts/" + std::string(myName);
		SCRIPT::ScriptLoader::LoadAll(*myScriptManager);
	}

	void SimpleNodeScript::Update()
	{
		mySimpleScriptWindow.Update(*myScriptManager, myName);
	}
}