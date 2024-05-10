#pragma once
#include "Engine/ECS/Core/EntityManager.hpp"

namespace ECS
{
	class System
	{
	public:
		System(EntityManager* aEntityManager) : myEntityManager(aEntityManager) {};

		virtual ~System() {};

		virtual void Init() {};
		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void EarlyUpdate() {};
		virtual void FixedUpdate() {};
		virtual void LateUpdate() {};

	protected:
		EntityManager* myEntityManager;
	};
}