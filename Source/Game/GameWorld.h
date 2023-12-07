#pragma once

class Model;

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	void Init();
	void Render();
private:
	std::vector<std::unique_ptr<Model>> myModels;
};