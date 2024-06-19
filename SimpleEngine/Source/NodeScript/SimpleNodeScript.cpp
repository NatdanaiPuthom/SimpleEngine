#include "SimpleNodeScript.hpp"
#include "ScriptFoundation.h"
#include "ScriptLoader.h"

namespace SCRIPT
{
	SimpleNodeScript::SimpleNodeScript()
	{
	}

	SimpleNodeScript::~SimpleNodeScript()
	{
		ScriptFoundation::GetInstance().Destroy();
	}

	void SimpleNodeScript::Init()
	{
		ScriptFoundation::GetInstance().InitializeSystemTypes();
		RegisterSimpleGameNodes();

		myName = "world_Middle";
		SCRIPT::ScriptLoader::SavePath = "../Source/Script/data/SimpleScripts/" + std::string(myName);
		SCRIPT::ScriptLoader::LoadAll();
	}

	void SimpleNodeScript::Update()
	{
		mySimpleScriptWindow.Update(myName);
	}
}