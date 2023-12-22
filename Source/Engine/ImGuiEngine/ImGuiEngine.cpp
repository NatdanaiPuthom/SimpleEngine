#include "Engine/stdafx.h"
#include "Engine/global.hpp"
#include "Engine/ImGuiEngine/ImGuiEngine.hpp"

#include <External/dearimgui/imgui/imgui.h>
#include <External/dearimgui/imgui/imgui_impl_win32.h>
#include <External/dearimgui/imgui/imgui_impl_dx11.h>
#include <External/dearimgui/imnodes/imnodes.h>

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

	ImGuiWindowFlags windowFlags = 
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceID = ImGui::GetID("InvisibleWindowDockSpace");

	ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
}

void ImGuiEngine::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}
