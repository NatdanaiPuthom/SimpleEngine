#pragma once

class Mesh;

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	void Init();
	void Render();
private:
	std::vector<std::unique_ptr<Mesh>> myMeshes;
};