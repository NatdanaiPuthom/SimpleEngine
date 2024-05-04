#pragma once
#include "SimpleScript/Core/Contexts/ExecutionContextBase.h"
#include "Game/GameCommon.h"

class Game;
class ScriptComponent;

struct SimpleGameContext : SCRIPT::ExecutionContextBase
{
	Game& game;
};

template<typename T>
struct SimpleOwnerContext : SimpleGameContext
{
	T* const currentOwner;
	T* const interaction;
};

void RegisterSimpleGameNodes();