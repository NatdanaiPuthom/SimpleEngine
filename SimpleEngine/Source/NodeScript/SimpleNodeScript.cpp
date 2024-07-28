#include "SimpleNodeScript.hpp"
#include "FlyFoundation.hpp"
#include "Fly.hpp"
#include "NodeScript/SimpleScript/SimpleGameNodes.hpp"

SimpleNodeScript::SimpleNodeScript()
{
}

SimpleNodeScript::~SimpleNodeScript()
{
	Fly::Foundation::GetInstance().Destroy();
}

void SimpleNodeScript::Init()
{
	Fly::Foundation::GetInstance().Initialize();
	RegisterSimpleGameNodes();

	Fly::LoadAllClasses(SCRIPT_FILE_PATH);
}
