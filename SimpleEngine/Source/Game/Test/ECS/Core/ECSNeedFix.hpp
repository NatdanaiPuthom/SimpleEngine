#pragma once
#include "Game/Test/ECS/Core/ComponentManager.hpp"

//NOTE(v9.27): I have to figure another structure where ECS include EntityManager which includes Entity which include ECS. Circle dependency.
//NOTE(v9.27): I do not want to forward declare as it would require places where ECS is begin used to be needed to include Entity.
//NOTE(v9.27): Pch header won't solve this issue as other 'Projects' may also use ECS. I would like it to be just include once and ready to use.
//NOTE(v9.27): No, I do not want to make them one big header as I would like to seperate them into different files as they itself are quite huge
//NOTE(v9.27): Please send help

namespace Disgusting
{
	Simple::ComponentManager* GetComponentManager();
}