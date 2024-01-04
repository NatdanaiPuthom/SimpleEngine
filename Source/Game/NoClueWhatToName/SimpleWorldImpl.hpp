#pragma once

namespace Simple
{
	class GameWorld;
	class LevelManager;
	class EventManager;
}

namespace Impl
{
	class SimpleWorldGameWorld final
	{
		friend class Simple::GameWorld;
	private:
		static void SetGameWorld(Simple::GameWorld* aGameWorld);
	};

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