#pragma once

struct GameLevel;
class Game;

struct GameUpdateContext
{
	float deltaTime;
	GameLevel& gameLevel;
	Game* game;
};