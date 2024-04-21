#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"

#include <External/dearimgui/imgui/imgui.h>
#include <External/dearimgui/imgui/imgui_impl_dx11.h>
#include <External/dearimgui/imgui/imgui_impl_win32.h>
#include <External/dearimgui/imnodes/imnodes.h>

namespace Simple
{
	ImGuiEngine::ImGuiEngine()
	{
	}

	ImGuiEngine::~ImGuiEngine()
	{
		const std::string output = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_IMGUI);
		ImGui::SaveIniSettingsToDisk(output.c_str());

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();

		ImGui::DestroyContext();
		ImNodes::DestroyContext();
	}

	void ImGuiEngine::Init()
	{
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImNodes::CreateContext();

		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;

		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_IMGUI);
		ImGui::LoadIniSettingsFromDisk(filename.c_str());

		ImGui_ImplWin32_Init(Global::GetEngineHWND());
		ImGui_ImplDX11_Init(Global::GetGraphicsEngine()->GetDevice().Get(), Global::GetGraphicsEngine()->GetContext().Get());

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.40f, 1.0f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.40f, 0.40f, 0.40f, 1.0f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.00f, 0.70f, 0.70f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
		colors[ImGuiCol_TabActive] = ImVec4(0.80f, 0.00f, 0.80f, 0.90f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);
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

#ifndef _SIMPLE //Draw ImGui Windows
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); 
#endif
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	void ImGuiEngine::Save()
	{
		const std::string output = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_IMGUI);
		ImGui::SaveIniSettingsToDisk(output.c_str());
	}
}