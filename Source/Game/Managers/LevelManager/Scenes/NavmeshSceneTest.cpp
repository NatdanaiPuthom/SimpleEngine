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
	SimpleWorld::GetEventmanager()->GetPostMaster().AddObserver(this, Simple::eEvent::Raycast_LMB);
	SimpleWorld::GetEventmanager()->GetPostMaster().AddObserver(this, Simple::eEvent::Raycast_RMB);

	auto navmesh = SimpleWorld::GetNavmesh();
	myPathFinder.SetNavmesh(navmesh);
}

void Scenes::NavmeshSceneTest::Update()
{
}

void Scenes::NavmeshSceneTest::Render()
{
	myPathFinder.RenderStartEndPosition();
	myPathFinder.RenderAStarPath();
	myPathFinder.RenderFunnelPath();

	auto navmesh = SimpleWorld::GetNavmesh();
	navmesh->RenderConnections();
	navmesh->RenderNavmesh();
}

void Scenes::NavmeshSceneTest::ReceiveMessage(const Simple::Message<Simple::eEvent>& aMessage)
{
	switch (aMessage.myType)
	{
	case Simple::eEvent::Raycast_LMB:
	{
		myEndPosition = std::any_cast<SU::Vector3f>(aMessage.myData);
		myPathFinder.CalculatePath(myStartPosition, myEndPosition);
		break;
	}
	case Simple::eEvent::Raycast_RMB:
	{
		myStartPosition = std::any_cast<SU::Vector3f>(aMessage.myData);
		myPathFinder.SetStartPosition(myStartPosition);
		break;
	}
	default:
		break;
	}
}
