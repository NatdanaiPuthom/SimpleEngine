#include "SimpleNodeScript.hpp"
#include "ScriptFoundation.hpp"
#include "ScriptLoader.hpp"
#include "NodeScript/SimpleScript/SimpleGameNodes.hpp"

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

		SCRIPT::ScriptLoader::LoadAllClasses(SCRIPT_FILE_PATH);
	}
}