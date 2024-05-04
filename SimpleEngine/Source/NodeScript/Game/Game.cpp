#include "Game.h"
#include <Game/GameCommon.h>
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
	if (mySceneChanger)
	{
		LoadLevel(mySceneChanger.mySceneName.c_str(), true);
		mySceneChanger.myChangeScene = false;
	}

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

void Game::ChangeScene(const SceneChanger3000& aSceneChanger)
{
	mySceneChanger = aSceneChanger;
	mySceneChanger.myChangeScene = true;
}

const SceneChanger3000& Game::GetSceneChanger() const
{
	return mySceneChanger;
}