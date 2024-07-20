#include "SimpleNodeScript.hpp"
#include "ScriptFoundation.hpp"
#include "ScriptLoader.hpp"
#include "NodeScript/SimpleScript/SimpleGameNodes.hpp"

SimpleNodeScript::SimpleNodeScript()
{
}

SimpleNodeScript::~SimpleNodeScript()
{
	Fly::ScriptFoundation::GetInstance().Destroy();
}

void SimpleNodeScript::Init()
{
	Fly::ScriptFoundation::GetInstance().Initialize();
	RegisterSimpleGameNodes();

	Fly::ScriptLoader::LoadAllClasses(SCRIPT_FILE_PATH);
}
