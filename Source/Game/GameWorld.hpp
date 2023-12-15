#pragma once

class Mesh;
class Renderer;

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	void Init();
	void Render();
private:
	std::unique_ptr<Renderer> myRenderer;
};