#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/LevelManager/Scenes/NavmeshSceneTest.hpp"
#include "Game/Navmesh/Navmesh.hpp"

Scenes::NavmeshSceneTest::NavmeshSceneTest()
{
}

Scenes::NavmeshSceneTest::~NavmeshSceneTest()
{
}

void Scenes::NavmeshSceneTest::Init()
{
	Simple::Navmesh navmesh;
	navmesh.Init("natdanaitestscene.obj");
}

void Scenes::NavmeshSceneTest::Update()
{
}