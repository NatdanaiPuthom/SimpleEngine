#pragma once

class GameWorld;

class SimpleWorldGameWorldImpl final
{
	friend class GameWorld;
private:
	static void SetGameWorld(GameWorld* aGameWorld);
};