#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "External/dearimgui/imgui/imgui_impl_dx11.h"
#include "External/dearimgui/imgui/imgui_impl_win32.h"
#include "External/dearimgui/imnodes/imnodes.h"
#include "External/dearimgui/imguizmo/ImGuizmo.h"
#include "External/AwsomeFontIcons/IconFontDefines.h"
#include <shellapi.h>

namespace Simple
{
	ImGuiStyle ImGuiEngine::myStyle = {};
	ImVec4 ImGuiEngine::myPlayModeBackgroundColor = {};
	ImVec4 ImGuiEngine::myInteractiveColor = {};
	const ImVec4 ImGuiEngine::myLegacyPinkColor = ImVec4(1.00f, 0.00f, 1.00f, 1.0f);
	const ImVec4 ImGuiEngine::mySoftBlueColor = ImVec4(0.29f, 0.56f, 0.89f, 1.0f);
	const ImVec4 ImGuiEngine::myDarkGrayColor = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
	ImVec4 ImGuiEngine::myColors[static_cast<size_t>(ImGuiCol_COUNT)] = {};
	eImGuiEditorStyle ImGuiEngine::myCurrentColorStyle = eImGuiEditorStyle::Simple;

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
		ImGuizmo::BeginFrame();

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

	void ImGuiEngine::SetEditorStyle(const eImGuiEditorStyle aStyle)
	{
		myCurrentColorStyle = aStyle;

		switch (myCurrentColorStyle)
		{
		case eImGuiEditorStyle::Simple:
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4* colors = ImGui::GetStyle().Colors;

			memcpy(&style, &myStyle, sizeof(myStyle));
			memcpy(colors, myColors, sizeof(myColors));
			break;
		}
		case eImGuiEditorStyle::Dark:
		{
			ImGui::StyleColorsDark();
			break;
		}
		case eImGuiEditorStyle::Light:
		{
			ImGui::StyleColorsLight();
			break;
		}
		default:
			break;
		}
	}

	void ImGuiEngine::SetEditorMode(const eImGuiEditorMode aMode)
	{
		switch (aMode)
		{
		case eImGuiEditorMode::Playing:
		{
			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_WindowBg] = myPlayModeBackgroundColor;
			break;
		}
		case eImGuiEditorMode::Default:
			SetEditorStyle(myCurrentColorStyle);
			break;
		default:
			break;
		}
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
		myPlayModeBackgroundColor = ImVec4(0.2350f, 0.05f, 0.05f, 1.0f);
		myInteractiveColor = mySoftBlueColor;

		const ImVec4 darkGrey = myDarkGrayColor;
		const ImVec4 lightGreyAlphaDown = ImVec4(0.485f, 0.485f, 0.485f, 0.90f);
		const ImVec4 darkGreyScaledDown = ImVec4(darkGrey.x - 0.04f, darkGrey.y - 0.04f, darkGrey.z - 0.04f, 1.00f);
		const ImVec4 darkGreySlightlyScaledUp = ImVec4(darkGrey.x + 0.05f, darkGrey.y + 0.05f, darkGrey.z + 0.05f, 1.0f);
		const ImVec4 darkGreyModerateScaledUp = ImVec4(darkGrey.x + 0.13f, darkGrey.y + 0.13f, darkGrey.z + 0.13f, 1.00f);
		const ImVec4 darkGreyMuchScaledUpAlphaDown = ImVec4(darkGrey.x + 0.20f, darkGrey.y + 0.20f, darkGrey.z + 0.20f, 0.90f);
		const ImVec4 darkGreyAlphaSlightlyScaledDown = ImVec4(darkGrey.x - 0.02f, darkGrey.x - 0.02f, darkGrey.x - 0.02f, 0.90f);
		const ImVec4 darkGreyAlphaModerateScaledDown = ImVec4(darkGreyAlphaSlightlyScaledDown.x, darkGreyAlphaSlightlyScaledDown.y, darkGreyAlphaSlightlyScaledDown.z, 0.80f);

		const ImVec4 activeColor = myInteractiveColor;
		const ImVec4 activeLessAlpha = ImVec4(activeColor.x, activeColor.y, activeColor.z, 0.90f);
		const ImVec4 activeHalfAlpha = ImVec4(activeColor.x, activeColor.y, activeColor.z, 0.50f);
		const ImVec4 activeScaledDown = ImVec4(activeColor.x * 0.70f, activeColor.y * 0.70f, activeColor.z * 0.70f, 0.70f);
		const ImVec4 activeScaledDownEqually = ImVec4(activeColor.x * 0.60f, activeColor.y * 0.60f, activeColor.z * 0.60f, 0.50f);
		const ImVec4 activeLessAlphaSlightlyGreen = ImVec4(activeColor.x * 0.90f, activeColor.y > 0 ? activeColor.y * 1.05f : 0.25f, activeColor.z, 0.40f);

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_MenuBarBg] = darkGrey;
		colors[ImGuiCol_WindowBg] = darkGrey;
		colors[ImGuiCol_PopupBg] = darkGreyScaledDown;
		colors[ImGuiCol_TitleBgActive] = activeColor;
		colors[ImGuiCol_TitleBgCollapsed] = activeColor;
		colors[ImGuiCol_ButtonHovered] = activeScaledDown;
		colors[ImGuiCol_HeaderHovered] = activeColor;
		colors[ImGuiCol_TabUnfocusedActive] = darkGreyMuchScaledUpAlphaDown;
		colors[ImGuiCol_Border] = darkGreyScaledDown;
		colors[ImGuiCol_FrameBg] = darkGreySlightlyScaledUp;
		colors[ImGuiCol_FrameBgHovered] = activeLessAlphaSlightlyGreen;
		colors[ImGuiCol_TitleBg] = darkGreyAlphaModerateScaledDown;
		colors[ImGuiCol_ScrollbarBg] = darkGrey;
		colors[ImGuiCol_CheckMark] = activeColor;
		colors[ImGuiCol_SliderGrab] = activeColor;
		colors[ImGuiCol_SliderGrabActive] = activeColor;
		colors[ImGuiCol_Button] = activeColor;
		colors[ImGuiCol_Header] = activeHalfAlpha;
		colors[ImGuiCol_Tab] = darkGreyModerateScaledUp;
		colors[ImGuiCol_TabHovered] = activeLessAlpha;
		colors[ImGuiCol_TabActive] = activeColor;
		colors[ImGuiCol_TabUnfocused] = darkGreyAlphaSlightlyScaledDown;
		colors[ImGuiCol_TabUnfocusedActive] = lightGreyAlphaDown;
		colors[ImGuiCol_DockingPreview] = activeColor;
		colors[ImGuiCol_ResizeGripHovered] = activeColor;
		colors[ImGuiCol_ResizeGripActive] = activeColor;
		colors[ImGuiCol_ButtonActive] = activeScaledDownEqually;
		colors[ImGuiCol_ChildBg] = darkGreyAlphaModerateScaledDown;

		for (size_t i = 0; i < static_cast<size_t>(ImGuiCol_COUNT); ++i)
		{
			myColors[i] = colors[i];
		}
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
}