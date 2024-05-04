#pragma once
#include "SimpleScript/Core/Contexts/ExecutionContextBase.h"
#include "Game/GameCommon.h"

struct GameLevel;
class Game;
class ScriptComponent;

struct SimpleGameContext : SCRIPT::ExecutionContextBase
{
	GameLevel& gameLevel;
	Game& game;
};

template<typename T>
struct SimpleOwnerContext : SimpleGameContext
{
	T* const currentOwner;
	T* const interaction;
};

struct SpriteTypeData
{
	SpriteId id;
};

void RegisterSimpleDataTypes();
void RegisterSimpleGameNodes();