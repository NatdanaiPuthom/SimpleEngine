#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/Scenes/NavmeshSceneTest.hpp"

Scenes::NavmeshSceneTest::NavmeshSceneTest()
{
}

Scenes::NavmeshSceneTest::~NavmeshSceneTest()
{
}

void Scenes::NavmeshSceneTest::OnEnter()
{
	auto camera = Global::GetGraphicsEngine()->GetDefaultCamera();
	camera->SetRotation(Math::Vector3f(40, -140, 0));
	camera->SetPosition(Math::Vector3f(20, 60, 20));
}

void Scenes::NavmeshSceneTest::Init()
{
	World::GetEventmanager()->GetPostMaster().AddObserver(this, Simple::eEvent::Raycast_LMB);
	World::GetEventmanager()->GetPostMaster().AddObserver(this, Simple::eEvent::Raycast_RMB);

	auto navmesh = World::GetNavmesh();
	navmesh->Init("Level 1.obj");
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
