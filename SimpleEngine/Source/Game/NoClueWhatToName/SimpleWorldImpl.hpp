#pragma once

namespace Simple
{
	class LevelManager;
	class EventManager;
}
namespace ECS
{
	class EntityComponentSystem;
}

namespace Impl
{
	class SimpleWorldLevelManager final
	{
		friend class Simple::LevelManager;
	private:
		static void SetLevelManager(Simple::LevelManager* aLevelManager);
	};

	class SimpleWorldEventManager final
	{
		friend class Simple::EventManager;
	private:
		static void SetEventManager(Simple::EventManager* aLevelManager);
	};

	class SimpleWorldECS final
	{
		friend class ECS::EntityComponentSystem;
	private:
		static void SetECS(ECS::EntityComponentSystem* aECS);
	};
}