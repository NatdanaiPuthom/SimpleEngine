#include "Game/stdafx.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/Tools/CameraTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/StatsTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/LightTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/MeshTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/SceneTool.hpp"
#include <External/imgui.h>

GameWorld::GameWorld()
	: myRenderer(std::make_unique<Renderer>())
	, myImGuiManager(std::make_unique<ImGuiManager>())
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	myImGuiManager->SetRenderer(myRenderer.get());

	myImGuiManager->AddTool(std::move(std::make_unique<CameraTool>()));
	myImGuiManager->AddTool(std::move(std::make_unique<StatsTool>()));
	myImGuiManager->AddTool(std::move(std::make_unique<LightTool>()));
	myImGuiManager->AddTool(std::move(std::make_unique<MeshTool>(myRenderer.get())));
	myImGuiManager->AddTool(std::move(std::make_unique<SceneTool>()));

	{ //Test stuff
		std::unique_ptr<Mesh> pyramid = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> cube = std::make_unique<Mesh>();

		MeshData pyramidData = ShapeCreator3000::CreatePyramid();
		MeshData cubeData = ShapeCreator3000::CreateCube();

		if (!pyramid->Init(pyramidData))
			assert(false && "Failed to create Pyramid");

		if (!cube->Init(cubeData))
			assert(false && "Failed to create Cube");

		if (!pyramid->AddTexture(0, "Assets/Textures/Cat.dds"))
			assert(false && "Failed To Add Texture");

		pyramid->SetScale({ 1,1,1 });
		pyramid->SetPosition(SimpleUtilities::Vector3f(-8, 2, 3));
		pyramid->SetName("Pyramid");

		cube->SetScale({ 5,5,5 });
		cube->SetPosition(SimpleUtilities::Vector3f(-7, 2, 10));
		cube->SetName("Cube");

		myRenderer->AddMesh(std::move(pyramid));
		myRenderer->AddMesh(std::move(cube));
	}
}

void GameWorld::Update()
{
	while (SimpleGlobal::GetGameIsRunning())
	{
	}
}

void GameWorld::Render()
{
	myRenderer->Render();
}

void GameWorld::RenderImGui()
{
	myImGuiManager->Render();
}
