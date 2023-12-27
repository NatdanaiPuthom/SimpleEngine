#pragma once

class GameWorld;
class LevelManager;

class SimpleWorldGameWorldImpl final
{
	friend class GameWorld;
private:
	static void SetGameWorld(GameWorld* aGameWorld);
};

class SimpleWorldLevelManagerImpl final
{
	friend class LevelManager;
private:
	static void SetLevelManager(LevelManager* aLevelManager);
};