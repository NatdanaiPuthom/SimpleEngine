#pragma once

constexpr int LEVEL_WIDTH = 16;
constexpr int LEVEL_HEIGHT = 9;

enum class SpriteId
{
	Grass,
	Wall,
	WallLeft,
	WallRight,
	WallTop,
	WallTopLeft,
	WallTopRight,
	WallSpecial,
	WallSpecialLeft,
	WallSpecialRight,
	WallSpecialTop,
	WallSpecialTopLeft,
	WallSpecialTopRight,
	PlateDown,
	PlateUp,
	YellowDoor,
	YellowKey,
	Coin,
	Flag,
	Ball,
	CharacterRed,
	CharacterBlue,
	Count
};

inline const char* GetSpriteNames()
{
	return "Grass\0Wall\0WallLeft\0WallRight\0WallTop\0WallTopLeft\0WallTopRight\0WallSpecial\0WallSpecialLeft\0WallSpecialRight\0WallSpecialTop\0WallSpecialTopLeft\0WallSpecialTopRight\0PlateDown\0PlateUp\0YellowDoor\0YellowKey\0Coin\0Flag\0Ball\0CharacterRed\0CharacterBlue";
}

enum class LevelTileType
{
	Grass,
	Wall,
	WallSpecial
};
