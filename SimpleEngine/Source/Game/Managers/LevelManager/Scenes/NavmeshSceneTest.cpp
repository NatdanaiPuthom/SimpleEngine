#include "Game/Precomplied/GamePch.hpp"
#include "Game/Managers/LevelManager/Scenes/NavmeshSceneTest.hpp"

Scenes::NavmeshSceneTest::NavmeshSceneTest()
{
}

Scenes::NavmeshSceneTest::~NavmeshSceneTest()
{
}

void Scenes::NavmeshSceneTest::Init()
{
	World::GetEventmanager()->GetPostMaster().AddObserver(this, Simple::eEvent::Raycast_LMB);
	World::GetEventmanager()->GetPostMaster().AddObserver(this, Simple::eEvent::Raycast_RMB);

	auto navmesh = World::GetNavmesh();
	myPathFinder.SetNavmesh(navmesh);
}

void Scenes::NavmeshSceneTest::Update()
{
	Scene::Update();
}

void Scenes::NavmeshSceneTest::Render()
{
	Scene::Render();

	myPathFinder.RenderStartEndPosition();
	myPathFinder.RenderAStarPath();
	myPathFinder.RenderFunnelPath();

	auto navmesh = World::GetNavmesh();
	navmesh->RenderConnections();
	navmesh->RenderNavmesh();
}

void Scenes::NavmeshSceneTest::ReceiveMessage(const Simple::Message<Simple::eEvent>& aMessage)
{
	switch (aMessage.myType)
	{
	case Simple::eEvent::Raycast_LMB:
	{
		myEndPosition = std::any_cast<Math::Vector3f>(aMessage.myData);
		myPathFinder.CalculatePath(myStartPosition, myEndPosition);
		break;
	}
	case Simple::eEvent::Raycast_RMB:
	{
		myStartPosition = std::any_cast<Math::Vector3f>(aMessage.myData);
		myPathFinder.SetStartPosition(myStartPosition);
		break;
	}
	default:
		break;
	}
}
