#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/LevelManager/Scenes/NavmeshSceneTest.hpp"

Scenes::NavmeshSceneTest::NavmeshSceneTest()
{
}

Scenes::NavmeshSceneTest::~NavmeshSceneTest()
{
}

void Scenes::NavmeshSceneTest::Init()
{
	myNavmesh.Init("natdanaitestscene.obj");
}

void Scenes::NavmeshSceneTest::Update()
{
}

void Scenes::NavmeshSceneTest::Render()
{
	myNavmesh.RenderNavmesh();
	myNavmesh.RenderConnections();
}
