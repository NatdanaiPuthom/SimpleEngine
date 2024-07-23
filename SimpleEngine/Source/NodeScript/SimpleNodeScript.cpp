#include "SimpleNodeScript.hpp"
#include "FlyFoundation.hpp"
#include "Fly.hpp"
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

	Fly::LoadAllClasses(SCRIPT_FILE_PATH);
}
