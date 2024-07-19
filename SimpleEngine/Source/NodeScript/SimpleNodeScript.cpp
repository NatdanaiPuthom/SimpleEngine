#include "SimpleNodeScript.hpp"
#include "ScriptFoundation.h"
#include "ScriptLoader.h"
#include "NodeScript/SimpleScript/SimpleGameNodes.h"

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
		ScriptFoundation::GetInstance().Initialize();
		RegisterSimpleGameNodes();

		SCRIPT::ScriptLoader::LoadAllScripts(SCRIPT_FILE_PATH);
	}
}