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
	camera->SetRotation(Math::Vector3(40.0f, -140.0f, 0.0f));
	camera->SetPosition(Math::Vector3(20.0f, 60.0f, 20.0f));

	myDirectionalLight->myModel->SetPosition({ 20.0f, 6.0f, 2.0f });
	myDirectionalLight->myModel->SetRotation({ 0.0f, 0.0f, 0.0f });
	Global::GetGraphicsEngine()->SetDirectionalLightDirection({ 0.0f, 0.0f, 1.0f });
}

void Scenes::NavmeshSceneTest::Init()
{
	World::GetEventmanager()->GetPostMaster().AddObserver(this, Simple::eEvent::Raycast_LMB);
	World::GetEventmanager()->GetPostMaster().AddObserver(this, Simple::eEvent::Raycast_RMB);

	auto navmesh = World::GetNavmesh();
	navmesh->Init("Level 1.obj");
	myPathFinder.SetNavmesh(navmesh);

	myGrid.Init(10, 100, 100, -50.0f, eGridRotate::Inverse_X | eGridRotate::Inverse_Z);
}

void Scenes::NavmeshSceneTest::Update()
{
	Scene::Update();

	if (myPathFinder.GetAStarPath().size() > 0 && myPathFinder.GetFunnelPath().size() > 0)
	{
		myShouldRenderPathfinder = true;
	}
	else
	{
		myShouldRenderPathfinder = false;
	}
}

void Scenes::NavmeshSceneTest::Render()
{
	Scene::Render();

	if (myShouldRenderPathfinder == true)
	{
		myPathFinder.RenderStartEndPosition();
		myPathFinder.RenderAStarPath();
		myPathFinder.RenderFunnelPath();
	}

	auto navmesh = World::GetNavmesh();
	navmesh->RenderConnections();
	navmesh->RenderNavmesh();

	myGrid.Render();
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
