#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"

#include <External/dearimgui/imgui/imgui.h>
#include <External/dearimgui/imgui/imgui_impl_dx11.h>
#include <External/dearimgui/imgui/imgui_impl_win32.h>
#include <External/dearimgui/imnodes/imnodes.h>

namespace Simple
{
#ifndef _SIMPLE
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
#else
	ImGuiEngine::ImGuiEngine() {}
	ImGuiEngine::~ImGuiEngine() {}

	void ImGuiEngine::Init() {}
	void ImGuiEngine::BeginFrame() {}
	void ImGuiEngine::EndFrame() {}
	void ImGuiEngine::Save() {}
#endif
}