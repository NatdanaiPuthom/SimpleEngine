#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/Scenes/NavmeshSceneTest.hpp"

Scenes::NavmeshSceneTest::NavmeshSceneTest()
	: myShouldRenderPathfinder(false)
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

	myGrid.Init(10, 100, 100);
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
	if (ImGui::Begin("Grid"))
	{
		static int height = 0;
		static int cellSize = static_cast<int>(myGrid.myCellSize);
		static int gridSize = static_cast<int>(myGrid.myRowAmount);
		static bool inverse_x = false;
		static bool inverse_z = false;
		static bool changed = false;
		static eGridRotate rotate = eGridRotate::None;

		if (ImGui::DragInt("CellSize", &cellSize, 1.0f, 1))
		{
			if (cellSize > 0)
			{
				myGrid.myCellSize = cellSize;
				changed = true;
			}
			else
			{
				cellSize = 0;
				myGrid.myCellSize = cellSize;
				changed = true;
			}
		}

		if (ImGui::DragInt("GridSize", &gridSize, 1.0f, 1))
		{
			if (gridSize > 0)
			{
				myGrid.myRowAmount = gridSize;
				myGrid.myColAmount = gridSize;
				changed = true;
			}
			else
			{
				gridSize = 0;
				myGrid.myRowAmount = gridSize;
				myGrid.myColAmount = gridSize;
				changed = true;
			}
		}

		if (ImGui::DragInt("Height", &height, 1.0f, 0))
		{
			changed = true;
		}

		if (ImGui::Checkbox("Inverse_X", &inverse_x))
		{
			rotate = eGridRotate::Inverse_X;
			changed = true;
		}

		if (ImGui::Checkbox("Inverse_Z", &inverse_z))
		{
			rotate = eGridRotate::Inverse_Z;
			changed = true;
		}

		if (changed)
		{
			myGrid.Init(myGrid.myCellSize, myGrid.myRowAmount, myGrid.myColAmount, static_cast<float>(height), rotate);
			changed = false;
		}
	}
	ImGui::End();

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
