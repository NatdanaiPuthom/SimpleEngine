#pragma once

namespace Simple
{
	class GameWorld;
	class LevelManager;
}

class SimpleWorldGameWorldImpl final
{
	friend class Simple::GameWorld;
private:
	static void SetGameWorld(Simple::GameWorld* aGameWorld);
};

class SimpleWorldLevelManagerImpl final
{
	friend class Simple::LevelManager;
private:
	static void SetLevelManager(Simple::LevelManager* aLevelManager);
};