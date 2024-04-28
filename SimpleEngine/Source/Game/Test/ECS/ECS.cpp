#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/ECS.hpp"

namespace Simple
{
	ECS::ECS()
		: myEntityManager(&myComponentManager)
	{
	}

	ECS::~ECS()
	{
	}

	void ECS::Init()
	{
	}
}