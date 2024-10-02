#include "Engine/Precomplied/EnginePch.hpp"
#include "SimpleNodeScript.hpp"
#include "NodeScript/Fly.hpp"

SimpleNodeScript::SimpleNodeScript()
{
}

SimpleNodeScript::~SimpleNodeScript()
{
	Fly::DestroyFoundation();
}

void SimpleNodeScript::Init()
{
	Fly::InitializeFoundation();

	Fly::LoadAllClasses(ASSET_FILE_PATH);
}
