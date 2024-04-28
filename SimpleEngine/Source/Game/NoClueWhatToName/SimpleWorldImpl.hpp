#pragma once

namespace Simple
{
	class LevelManager;
	class EventManager;
	class ComponentManager;
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
}