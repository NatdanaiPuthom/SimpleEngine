#pragma once

namespace Tga
{
	class InputManager;
}

struct GameLevel;
class Game;

/// <summary>
/// By inheriting from ScriptUpdateContext, GameUpdateContext can be used to provide additional data to nodes in this game
/// See usage in PlayerNode for example CreatePlayerNode for example
/// </summary>
struct GameUpdateContext
{
	float deltaTime;
	GameLevel& gameLevel;
	const Tga::InputManager& inputManager;
	Game* game;
};

struct GameInitContext
{
	GameLevel& gameLevel;
	Game* game;
};