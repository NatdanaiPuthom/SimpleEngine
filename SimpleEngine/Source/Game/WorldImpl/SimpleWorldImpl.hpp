#pragma once

namespace Simple
{
	class EventManager;
}
namespace ECS
{
	class EntityComponentSystem;
}

namespace Impl
{
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