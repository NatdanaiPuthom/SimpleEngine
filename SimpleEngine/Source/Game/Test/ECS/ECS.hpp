#pragma once
#include "Game/Test/ECS/Core/EntityManager.hpp"

namespace Simple
{
	class ECS final
	{
	public:
		ECS();
		~ECS();

		void Init();

	private:
		EntityManager myEntityManager;
		ComponentManager myComponentManager;
	};
}