#include "Editor/Precomplied/EditorPch.hpp"
#include "GraphicsSettingsPopUp.hpp"
#include "Engine/Global.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include <array>
#include <unordered_map>
#include <stdlib.h>
#include <windef.h>

namespace Editor
{
	GraphicsSettingsPopUp::GraphicsSettingsPopUp(const std::string& aName)
		: PopUp(aName)
		, mySelectedWindowSize(0)
		, mySelectedRasterizerState(0)
		, mySelectedCursor(0)
		, myConsoleIsOpen(true)
		, myRasterizerStatesConstChar({})
	{
	}

	void GraphicsSettingsPopUp::Init()
	{
		InitRasterizerStatesStrings();

		UpdateAndFetchFPSCapStrings();
		UpdateAndFetchCurrentMonitorResolution();
		UpdateAndFetchCurrentCursorSettings();
	}

	void GraphicsSettingsPopUp::Render()
	{
		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			constexpr unsigned int heightPadding = 2;

			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
			Graphics::GenericDataManager* graphicsGenericDataManager = graphicsEngine->GetGenericDataManager();
			Drawer::Renderer* renderer = graphicsEngine->GetRenderer();

			bool vsync = graphicsGenericDataManager->IsVSyncOn();
			bool shouldRenderDebugLines = renderer->GetShouldRenderDebugLines();
			bool shouldRenderBoundingBox = renderer->GetShouldRenderBoundingBox();
			bool shouldRenderMesh = renderer->GetShouldRenderMesh();
			bool shouldRenderSkeleton = renderer->GetShouldRenderSkeletonLines();
			bool isUsingPBR = renderer->GetIsUsingPBR();

			const std::string drawCalls = "DrawCalls: " + std::to_string(Global::GetDrawCalls());
			const std::string fps = "FPS: " + std::to_string(Global::GetFPS());

			ImGui::Text(drawCalls.c_str());
			ImGui::SameLine(ImGui::GetWindowWidth() - 100);
			ImGui::Text(fps.c_str());
			
			Simple::ImGuiEngine::SeparatorTextDummy(0, 0, "Render");

			static const std::string vSync = std::string("VSync").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(vSync.c_str(), &vsync))
			{
				graphicsGenericDataManager->SetVSync(vsync);
			}

			Simple::ImGuiEngine::SameLineDummy(3, 0);

			static const std::string debugLines = std::string("DebugLines").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(debugLines.c_str(), &shouldRenderDebugLines))
			{
				renderer->SetShouldRenderDebugLines(shouldRenderDebugLines);
			}

			Simple::ImGuiEngine::SameLineDummy(3, 0);

			static const std::string boundingBox = std::string("BoundingBox").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(boundingBox.c_str(), &shouldRenderBoundingBox))
			{
				renderer->SetShouldRenderBoundingBox(shouldRenderBoundingBox);
			}

			static const std::string mesh = std::string("Mesh").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(mesh.c_str(), &shouldRenderMesh))
			{
				renderer->SetShouldRenderMesh(shouldRenderMesh);
			}

			Simple::ImGuiEngine::SameLineDummy(10, 0);

			static const std::string pbr = std::string("PBR").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(pbr.c_str(), &isUsingPBR))
			{
				renderer->SetIsUsingPBR(isUsingPBR);
			}

			Simple::ImGuiEngine::SameLineDummy(52, 0);

			static const std::string skeleton = std::string("Skeleton").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(skeleton.c_str(), &shouldRenderSkeleton))
			{
				renderer->SetShouldRenderSkeletonLines(shouldRenderSkeleton);
			}

			Simple::ImGuiEngine::SeparatorDummy(0, heightPadding);

			UpdateFPSCapSettings(graphicsEngine);

			ImGui::SetNextItemWidth(200);

			static const std::string rasterizerState = std::string("RasterizerState").append(myImGuiTag).c_str();
			if (ImGui::Combo(rasterizerState.c_str(), &mySelectedRasterizerState, myRasterizerStatesConstChar.data(), static_cast<int>(myRasterizerStatesConstChar.size())))
			{
				graphicsEngine->GetStateManager()->SetRasterizerState(graphicsEngine->GetContext(), static_cast<Graphics::eRasterizerState>(mySelectedRasterizerState));
			}

			UpdateWindowsSizeSettings();

			Simple::ImGuiEngine::SeparatorDummy(0, heightPadding);

			UpdateConsoleSettings();

			Simple::ImGuiEngine::SeparatorDummy(0, heightPadding);

			static std::vector<const char*> editorStyles = { "Simple","Dark", "Light" };

			static int selectedStyle = 0;

			ImGui::SetNextItemWidth(200);

			static const std::string editorStyle = std::string("Editor Style").append(myImGuiTag).c_str();
			if (ImGui::Combo(editorStyle.c_str(), &selectedStyle, editorStyles.data(), static_cast<int>(editorStyles.size())))
			{
				switch (selectedStyle)
				{
				case 0:
					Simple::ImGuiEngine::SetEditorStyle(Simple::eImGuiEditorStyle::Simple);
					break;
				case 1:
					Simple::ImGuiEngine::SetEditorStyle(Simple::eImGuiEditorStyle::Dark);
					break;
				case 2:
					Simple::ImGuiEngine::SetEditorStyle(Simple::eImGuiEditorStyle::Light);
					break;
				default:
					Simple::ImGuiEngine::SetEditorStyle(Simple::eImGuiEditorStyle::Simple);
					break;
				}
			}

			Simple::ImGuiEngine::SeparatorDummy(0, heightPadding);
			UpdateCursorSettings();
			Simple::ImGuiEngine::SeparatorDummy(0, heightPadding);
		}

		ImGui::End();
	}

	void GraphicsSettingsPopUp::InitRasterizerStatesStrings()
	{
		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::BackfaceCulling)] = "BackfaceCulling";
		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::NoFaceCulling)] = "NoFaceCulling";
		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::Wireframe)] = "Wireframe";
		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::WireframeNoCulling)] = "WireframeNoCulling";
		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::FrontFaceCulling)] = "FrontFaceCulling";
	}

	void GraphicsSettingsPopUp::UpdateFPSCapSettings(Graphics::GraphicsEngine* aGraphicsEngine)
	{
		Graphics::GenericDataManager* genericDataManager = aGraphicsEngine->GetGenericDataManager();

		int selectedFPSLevelCap = genericDataManager->GetFPSLevelCap();

		if (selectedFPSLevelCap == 1)
		{
			ImGui::Text("FPS Capped: %s", myFPSCapAsConstChar[selectedFPSLevelCap]);
		}
		else
		{
			ImGui::SetNextItemWidth(200);

			if (ImGui::Combo("FPS Cap##SettingWindow", &selectedFPSLevelCap, myFPSCapAsConstChar.data(), static_cast<int>(myFPSCapAsConstChar.size())))
			{
				if (selectedFPSLevelCap == 1)
				{
					genericDataManager->SetVSync(true);
				}
				else
				{
					genericDataManager->SetFPSLevelCap(selectedFPSLevelCap);
				}
			}
		}
	}

	void GraphicsSettingsPopUp::UpdateConsoleSettings()
	{
		static const std::string console = std::string("Show Console").append(myImGuiTag).c_str();
		if (ImGui::Checkbox(console.c_str(), &myConsoleIsOpen))
		{
			HWND consoleWindow = GetConsoleWindow();

			if (myConsoleIsOpen)
			{
				ShowWindow(consoleWindow, SW_SHOW);
			}
			else
			{
				ShowWindow(consoleWindow, SW_HIDE);
			}
		}

		static const std::string clearConsole = std::string("Clear Console").append(myImGuiTag).c_str();
		if (ImGui::Button(clearConsole.c_str()))
		{
			system("CLS");
		}
	}

	void GraphicsSettingsPopUp::UpdateCursorSettings()
	{
		ImGui::SetNextItemWidth(200);

		static const std::string cursors = std::string("Cursors").append(myImGuiTag).c_str();
		if (ImGui::Combo(cursors.c_str(), &mySelectedCursor, myLongCursorStringName.c_str()))
		{
			Global::SetCustomCursor(myCursorNames[mySelectedCursor]);
		}
	}

	void GraphicsSettingsPopUp::UpdateWindowsSizeSettings()
	{
		Math::Vector2ui resolution = Global::GetResolution();

		if (myWindowSizes[mySelectedWindowSize].x != resolution.x && myWindowSizes[mySelectedWindowSize].y != resolution.y)
		{
			for (size_t i = 0; i < myWindowSizes.size(); ++i)
			{
				if (myWindowSizes[i].x == resolution.x && myWindowSizes[i].y == resolution.y)
				{
					mySelectedWindowSize = static_cast<int>(i);
					break;
				}
			}
		}

		ImGui::SetNextItemWidth(200);

		std::vector<std::string> windowSizeAsString;
		for (const auto& size : myWindowSizes)
		{
			const std::string string = std::to_string(size.x) + "x" + std::to_string(size.y);
			windowSizeAsString.push_back(string);
		}

		std::vector<const char*> windowSizeAsChar;
		for (const std::string& string : windowSizeAsString)
		{
			windowSizeAsChar.push_back(string.c_str());
		}

		if (ImGui::Combo("WindowSize##SettingWindow", &mySelectedWindowSize, windowSizeAsChar.data(), static_cast<int>(myWindowSizes.size())))
		{
			const auto currentWindowSize = Global::GetWindowSize();

			if (currentWindowSize.x != myWindowSizes[mySelectedWindowSize].x &&
				currentWindowSize.y != myWindowSizes[mySelectedWindowSize].y)
			{
				UpdateAndFetchCurrentMonitorResolution();

				if (myWindowSizes[mySelectedWindowSize].x == myMonitorResolution.x &&
					myWindowSizes[mySelectedWindowSize].y == myMonitorResolution.y)
				{
					Global::SetWindowSizeNextFrame(myMonitorResolution, true);
				}
				else
				{
					Global::SetWindowSizeNextFrame(myWindowSizes[mySelectedWindowSize]);
				}
			}
		}
	}

	void GraphicsSettingsPopUp::UpdateAndFetchCurrentMonitorResolution()
	{
		HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);

		MONITORINFOEX monitorInfo = { sizeof(MONITORINFOEX) };
		GetMonitorInfo(hMonitor, &monitorInfo);

		myMonitorResolution.x = static_cast<unsigned int>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);
		myMonitorResolution.y = static_cast<unsigned int>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);

		myWindowSizes.clear();
		myWindowSizes.push_back(Math::Vector2ui(1280, 720));
		myWindowSizes.push_back(Math::Vector2ui(1600, 900));
		myWindowSizes.push_back(Math::Vector2ui(1920, 1080));

		if (myWindowSizes.back().x < myMonitorResolution.x &&
			myWindowSizes.back().y < myMonitorResolution.y)
		{
			myWindowSizes.push_back(myMonitorResolution);
		}
	}

	void GraphicsSettingsPopUp::UpdateAndFetchFPSCapStrings()
	{
		myFPSCapAsString.clear();
		myFPSCapAsConstChar.clear();

		int monitorUpdateFrequency = 0;

		HDC hdc = GetDC(0);
		DEVMODE devMode;
		devMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

		monitorUpdateFrequency = static_cast<int>(devMode.dmDisplayFrequency);
		ReleaseDC(0, hdc);

		myFPSCapAsString.reserve(5);
		myFPSCapAsString.push_back("Uncapped");

		for (int i = 1; i <= 4; ++i)
		{
			const int hz = monitorUpdateFrequency / i;

			if (hz >= 60)
			{
				myFPSCapAsString.push_back(std::to_string(hz));
			}
		}

		myFPSCapAsConstChar.reserve(myFPSCapAsString.size());

		for (const std::string& fpsString : myFPSCapAsString)
		{
			myFPSCapAsConstChar.push_back(fpsString.c_str());
		}
	}

	void GraphicsSettingsPopUp::UpdateAndFetchCurrentCursorSettings()
	{
		myCursorNames.clear();
		myLongCursorStringName.clear();

		const std::unordered_map<std::string, const HCURSOR>& loadedCursors = Global::GetLoadedCustomCursors();
		const HCURSOR currentCursor = Global::GetCurrentCustomCursor();

		for (const auto& [name, cursor] : loadedCursors)
		{
			myCursorNames.push_back(name);
			myLongCursorStringName += name;
			myLongCursorStringName += '\0';
		}

		myLongCursorStringName += '\0';

		unsigned int index = 0;

		for (const auto& [name, cursor] : loadedCursors)
		{
			if (currentCursor == cursor)
			{
				mySelectedCursor = index;
				break;
			}

			index++;
		}
	}
}