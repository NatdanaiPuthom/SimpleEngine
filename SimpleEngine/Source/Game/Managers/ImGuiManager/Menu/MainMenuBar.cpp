#include "Game/Precomplied/GamePch.hpp"
#include "Game/Managers/ImGuiManager/Menu/MainMenuBar.hpp"
#include "Game/Managers/ImGuiManager/Tools/SettingsTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/RenderTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/MeshTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/CameraTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/LightTool.hpp"

Tool::MainMenuBar::MainMenuBar()
	: myNodeToolActive(false)
	, mySettingToolActive(false)
	, myRenderToolActive(false)
	, myCameraToolActive(false)
	, myMeshToolActive(false)
	, myLightToolActive(false)
{
}

void Tool::MainMenuBar::Init()
{
	mySettingsTool = std::make_unique<Tool::SettingsTool>();
	myRenderTool = std::make_unique<Tool::RenderTool>();
	myMeshTool = std::make_unique<Tool::MeshTool>();
	myCameraTool = std::make_unique<Tool::CameraTool>();
	myLightTool = std::make_unique<Tool::LightTool>();

	mySettingsTool->Init();
	myRenderTool->Init();
	myMeshTool->Init();
	myCameraTool->Init();
	myLightTool->Init();
}

void Tool::MainMenuBar::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("Settings", "", &mySettingToolActive);
			ImGui::MenuItem("Render Test", "", &myRenderToolActive);
			ImGui::MenuItem("Meshes", "", &myMeshToolActive);
			ImGui::MenuItem("Camera", "", &myCameraToolActive);
			ImGui::MenuItem("Light", "", &myLightToolActive);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Node"))
		{
			ImGui::MenuItem("Node Editor", "SHIFT + UNKNOWN", &myNodeToolActive);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (myNodeToolActive)
	{
		if (ImGui::Begin("Script Editor", &myNodeToolActive))
		{
			ImNodes::BeginNodeEditor();
			ImNodes::BeginNode(1);

			ImNodes::BeginNodeTitleBar();
			ImGui::TextUnformatted("Player");
			ImNodes::EndNodeTitleBar();

			ImNodes::BeginOutputAttribute(2);
			ImGui::Text("Idle");
			ImNodes::EndInputAttribute();

			ImNodes::EndNode();
			ImNodes::EndNodeEditor();
		}

		ImGui::End();
	}

	DrawTools();
}

void Tool::MainMenuBar::DrawTools()
{
	if (mySettingToolActive)
	{
		mySettingsTool->Draw();
	}

	if (myRenderToolActive)
	{
		myRenderTool->Draw();
	}

	if (myMeshToolActive)
	{
		myMeshTool->Draw();
	}

	if (myCameraToolActive)
	{
		myCameraTool->Draw();
	}

	if (myLightToolActive)
	{
		myLightTool->Draw();
	}
}