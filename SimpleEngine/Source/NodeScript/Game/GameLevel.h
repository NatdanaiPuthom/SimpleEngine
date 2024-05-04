#pragma once
#include <Game/GameCommon.h>
#include "GameObjects/GameObjectManager.h"
#include <memory>

namespace SCRIPT
{
	class ScriptManager;
}

struct GameLevel
{
	GameObjectManager gameObjectManager;
	SCRIPT::ScriptManager* scriptManager;
	std::string name;

	LevelTileType tiles[LEVEL_WIDTH][LEVEL_HEIGHT];

	LevelTileType GetTileType(Math::Vector2i pos) const
	{
		if (pos.x < 0 || pos.x >= LEVEL_WIDTH || pos.y < 0 || pos.y >= LEVEL_HEIGHT)
		{
			return LevelTileType::Wall;
		}
		else
		{
			return tiles[pos.x][pos.y];
		}
	}
};