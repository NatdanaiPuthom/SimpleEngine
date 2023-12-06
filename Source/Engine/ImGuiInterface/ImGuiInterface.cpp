#include "stdafx.h"
#include "Engine/global.h"
#include "Engine/ImGuiInterface/ImGuiInterface.h"

#include <External/dearimgui/imgui/imgui.h>
#include <External/dearimgui/imgui/imgui_impl_win32.h>
#include <External/dearimgui/imgui/imgui_impl_dx11.h>
#include <External/dearimgui/imnodes/imnodes.h>

ImGuiInterface::ImGuiInterface()
{
}

ImGuiInterface::~ImGuiInterface()
{
	const std::string output = SIMPLE_IMGUI_DIR + std::string(SIMPLE_IMGUI_FILENAME);
	ImGui::SaveIniSettingsToDisk(output.c_str());

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImNodes::DestroyContext();
	ImGui::DestroyContext();
}

void ImGuiInterface::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	const std::string output = SIMPLE_IMGUI_DIR + std::string(SIMPLE_IMGUI_FILENAME);
	ImGui::LoadIniSettingsFromDisk(output.c_str());

	ImNodes::CreateContext();
	ImGui_ImplWin32_Init(SimplyGlobal::GetHWND());
	ImGui_ImplDX11_Init(SimplyGlobal::GetGraphicsEngine()->GetDevice().Get(), SimplyGlobal::GetGraphicsEngine()->GetContext().Get());
}

void ImGuiInterface::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiInterface::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}
