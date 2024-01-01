#include "Engine/Precomplier/stdafx.h"
#include "Engine/ImGui/ImGuiEngine.hpp"


ImGuiEngine::ImGuiEngine()
{
}

ImGuiEngine::~ImGuiEngine()
{
	const std::string output = SimpleUtilities::GetPath(SIMPLE_IMGUI_FILENAME);
	ImGui::SaveIniSettingsToDisk(output.c_str());

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImNodes::DestroyContext();
	ImGui::DestroyContext();
}

void ImGuiEngine::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	const std::string output = SimpleUtilities::GetPath(SIMPLE_IMGUI_FILENAME);
	ImGui::LoadIniSettingsFromDisk(output.c_str());

	ImNodes::CreateContext();
	ImGui_ImplWin32_Init(SimpleGlobal::GetHWND());
	ImGui_ImplDX11_Init(SimpleGlobal::GetGraphicsEngine()->GetDevice().Get(), SimpleGlobal::GetGraphicsEngine()->GetContext().Get());
}

void ImGuiEngine::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImGuiEngine::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}
