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
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.00f, 0.70f, 0.70f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);
		colors[ImGuiCol_Border] = ImVec4(0.35f, 0.0f, 0.395f, 0.50f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.25f, 1.00f, 0.40f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.35f, 0.0f, 0.395f, 0.50f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.00f, 1.0f, 1.0f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.00f, 1.0f, 1.0f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.00f, 1.0f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(1.0f, 0.00f, 1.0f, 1.0f);
		colors[ImGuiCol_Header] = ImVec4(1.0f, 0.00f, 1.0f, 0.50f);
		colors[ImGuiCol_Tab] = ImVec4(0.50f, 0.00f, 0.50f, 0.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.0f, 0.30f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.0f, 0.3f, 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.35f, 0.35f, 0.35f, 0.0f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.6f, 0.0f, 0.6f, 0.50f);
		colors[ImGuiCol_DockingPreview] = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 0;
		style.TabBarBorderSize = 0;
		style.TabBorderSize = 1;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.DockingSeparatorSize = 1;
		style.DisplaySafeAreaPadding.x = 10;
		style.PopupBorderSize = 0;
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

	void ImGuiEngine::Save()
	{
		const std::string output = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_IMGUI);
		ImGui::SaveIniSettingsToDisk(output.c_str());
	}
}