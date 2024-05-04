#pragma once
#include "SimpleScript/Core/Contexts/ExecutionContextBase.h"

class Game;

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