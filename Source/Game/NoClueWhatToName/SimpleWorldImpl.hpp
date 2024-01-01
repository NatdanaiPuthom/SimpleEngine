#pragma once

namespace Simple
{
	class GameWorld;
	class LevelManager;
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
}