#pragma once
#include <vector>
#include <memory>

class Model;

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	void Render();
private:
	std::vector<std::unique_ptr<Model>> myModels;
};