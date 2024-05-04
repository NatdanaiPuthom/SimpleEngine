#include "Game.h"
#include "SimpleScript/SimpleGameNodes.h"
#include "SimpleScript/Core/Serialization/ScriptLoader.h"
#include <fstream>

Game::Game()
{
}

Game::~Game()
{}

void Game::Init()
{
	myScriptFoundation.InitializeSystemTypes();

	RegisterSimpleGameNodes();

	myGlobalScriptManager = &myScriptFoundation.CreateScriptManager();

	SCRIPT::ScriptLoader::SavePath = "../Source/Script/data/SimpleScripts/";
	SCRIPT::ScriptLoader::LoadAll(*myGlobalScriptManager);

	LoadLevel("world_Middle", false);
}

void Game::Update(float aTimeDelta)
{
	SimpleGameContext updateContext
	{
		aTimeDelta,
		*this,
	};
}

void Game::Render()
{
}

void Game::LoadLevel(const char* /*name*/, bool runScripts)
{
	if (runScripts)
	{
		SimpleGameContext updateContext
		{
			0.f,
			*this,
		};
	}
}