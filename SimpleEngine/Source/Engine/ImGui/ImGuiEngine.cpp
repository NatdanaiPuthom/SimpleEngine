#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "Engine/ImGui/IconFontDefines.h"
#include "External/dearimgui/imgui/imgui_impl_dx11.h"
#include "External/dearimgui/imgui/imgui_impl_win32.h"
#include "External/dearimgui/imnodes/imnodes.h"
#include <shellapi.h>

namespace Simple
{
	ImGuiStyle ImGuiEngine::myStyle = {};
	ImVec4 ImGuiEngine::myColors = {};

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
	}

	void ImGuiEngine::Init()
	{
		IMGUI_CHECKVERSION();

		DragAcceptFiles(Global::GetEngineHWND(), TRUE);

		ImGui::CreateContext();
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

		LoadColors();
		LoadStyle();
		LoadFronts();
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

	void ImGuiEngine::SetSimpleStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = ImGui::GetStyle().Colors;

		style = myStyle;
		colors = &myColors;
	}

	void ImGuiEngine::SetDarkStyle()
	{
		ImGui::StyleColorsDark();
	}

	void ImGuiEngine::SetLightStyle()
	{
		ImGui::StyleColorsLight();
	}

	void ImGuiEngine::LoadFronts()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();

		constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;

		io.Fonts->AddFontFromFileTTF(SimpleUtilities::GetAbsolutePath("Assets\\Fonts\\fa-solid-900.ttf").c_str(), 11.0f, &icons_config, icons_ranges);
		io.Fonts->Build();
	}

	void ImGuiEngine::LoadColors()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.00f, 0.70f, 0.70f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);
		colors[ImGuiCol_Border] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.25f, 1.00f, 0.40f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.18f, 0.18f, 0.80f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.00f, 1.0f, 1.0f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.00f, 1.0f, 1.0f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.00f, 1.0f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(1.0f, 0.00f, 1.0f, 1.0f);
		colors[ImGuiCol_Header] = ImVec4(1.0f, 0.00f, 1.0f, 0.50f);
		colors[ImGuiCol_Tab] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.0f, 1.00f, 0.90f);
		colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 0.90f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.485f, 0.485f, 0.485f, 0.90f);
		colors[ImGuiCol_DockingPreview] = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.6f, 0.0f, 0.6f, 0.50f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.18f, 0.18f, 0.18f, 0.80f);

		myColors = *colors;
	}

	void ImGuiEngine::LoadStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 0;
		style.TabBarBorderSize = 0;
		style.TabBorderSize = 0;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.DockingSeparatorSize = 1;
		style.DisplaySafeAreaPadding.x = 10;
		style.PopupBorderSize = 0;
		style.FrameBorderSize = 0.0f;
		style.ChildRounding = 5.0f;
		style.TabRounding = 0;

		myStyle = style;
	}

	void ImGuiEngine::ShowFAIcons()
	{
		if (ImGui::Begin("FAIcons##DemoFaIcons"))
		{
			ImGui::Button(ICON_FA_AD);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ADDRESS_BOOK);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ADDRESS_CARD);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ADJUST);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_AIR_FRESHENER);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALIGN_CENTER);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALIGN_JUSTIFY);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALIGN_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALIGN_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALLERGIES);

			ImGui::Button(ICON_FA_AMBULANCE);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_AMERICAN_SIGN_LANGUAGE_INTERPRETING);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANCHOR);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOUBLE_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOUBLE_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOUBLE_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOUBLE_UP);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_UP);

			ImGui::Button(ICON_FA_ANGRY);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANKH);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_APPLE_ALT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARCHIVE);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARCHWAY);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_UP);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_CIRCLE_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_CIRCLE_LEFT);

			ImGui::Button(ICON_FA_ARROW_CIRCLE_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_CIRCLE_UP);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_UP);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROWS_ALT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROWS_ALT_H);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROWS_ALT_V);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ASSISTIVE_LISTENING_SYSTEMS);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ASTERISK);

			ImGui::Button(ICON_FA_AT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ATLAS);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ATOM);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_AUDIO_DESCRIPTION);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_AWARD);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BABY);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BABY_CARRIAGE);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BACKSPACE);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BACKWARD);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BACON);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BACTERIA);

			//NOTE(v11.2.4): Fuck this, I will try to automate this one day but not today
		}
		ImGui::End();
	}
}