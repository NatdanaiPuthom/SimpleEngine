#pragma once
#include "SimpleScript/Core/Contexts/ExecutionContextBase.h"

struct SimpleGameContext : SCRIPT::ExecutionContextBase
{
};

template<typename T>
struct SimpleOwnerContext : SimpleGameContext
{
	T* const currentOwner;
	T* const interaction;
};

void RegisterSimpleGameNodes();

