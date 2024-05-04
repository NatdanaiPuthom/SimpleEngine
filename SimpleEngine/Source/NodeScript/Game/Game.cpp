#include "Game.h"
#include "GameObjects/Base/GameObject.h"

#include <Game/GameCommon.h>
#include <Game/GameLevel.h>
#include <Game/GameUpdateContext.h>

#include <fstream>

#include "SimpleScript/SimpleGameNodes.h"
#include "SimpleScript/Core/Serialization/ScriptLoader.h"

Game::Game()
{
}

Game::~Game()
{}

void Game::Init()
{
	myScriptFoundation.InitializeSystemTypes();

	RegisterSimpleDataTypes();
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
		myCurrentLevel,
		*this,
	};

	myCurrentLevel.scriptManager->TriggerEvent(SCRIPT::eNodeExecutionTrait::Tick, updateContext);
}

void Game::Render()
{
}

void Game::LoadLevel(const char* name, bool runScripts)
{
	bool success = false;
	std::string path = "";//Tga::Settings::ResolveAssetPath("levels/" + std::string(name) + ".txt", &success);

	if (!success)
	{
		return;
	}

	myScriptFoundation.DestroyScriptManager(*myCurrentLevel.scriptManager);
	myCurrentLevel = { };
	myCurrentLevel.scriptManager = &myScriptFoundation.CreateScriptManager();

	SCRIPT::ScriptLoader::SavePath = "../Source/Script/data/SimpleScripts/" + std::string(name);
	SCRIPT::ScriptLoader::LoadAll(*myCurrentLevel.scriptManager);

	{
		std::ifstream file(path);
		std::string line;

		int ballCount = 0;
		int specialBallCount = 0;
		int coinCount = 0;
		int flagCount = 0;

		char nameBuffer[128];

		int lineIndex = 0;

		while (std::getline(file, line) && lineIndex < LEVEL_HEIGHT)
		{
			for (int column = 0; column < LEVEL_WIDTH; column++)
			{
				LevelTileType t = LevelTileType::Grass;
				if (line[column] == '#')
				{
					t = LevelTileType::Wall;
				}
				if (line[column] == 'S')
				{
					t = LevelTileType::WallSpecial;
				}
				else if (line[column] == 'b')
				{
					sprintf_s(nameBuffer, "ball %03d", ballCount++);
				}
				else if (line[column] == 'v')
				{
					sprintf_s(nameBuffer, "BlueCharacter %03d", specialBallCount++);
				}
				else if (line[column] == 'c')
				{
					sprintf_s(nameBuffer, "coin %03d", coinCount++);
				}
				else if (line[column] == 'f')
				{
					sprintf_s(nameBuffer, "flag %03d", flagCount++);
				}
			}

			lineIndex++;
		}
	}

	if (runScripts)
	{
		SimpleGameContext updateContext
		{
			0.f,
			myCurrentLevel,
			*this,
		};

		myCurrentLevel.scriptManager->TriggerEvent(SCRIPT::eNodeExecutionTrait::BeginPlay, updateContext);
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