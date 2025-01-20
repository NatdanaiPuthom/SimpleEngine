#include "Editor/Precomplied/EditorPch.hpp"
#include "GraphicsSettingsPopUp.hpp"
#include "Engine/Global.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include <array>

namespace Editor
{
	GraphicsSettingsPopUp::GraphicsSettingsPopUp(const std::string& aName)
		: PopUp(aName)
		, mySelectedRasterizerState(0)
		, myConsoleIsOpen(true)
		, myRasterizerStatesConstChar({})
	{
	}

	void GraphicsSettingsPopUp::Init()
	{
		int monitorUpdateFrequency = 0;

		{
			HDC hdc = GetDC(0);
			DEVMODE devMode;
			devMode.dmSize = sizeof(DEVMODE);
			EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

			monitorUpdateFrequency = static_cast<int>(devMode.dmDisplayFrequency);
			ReleaseDC(0, hdc);
		}

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

		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::BackfaceCulling)] = "BackfaceCulling";
		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::NoFaceCulling)] = "NoFaceCulling";
		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::Wireframe)] = "Wireframe";
		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::WireframeNoCulling)] = "WireframeNoCulling";
		myRasterizerStatesConstChar[static_cast<int>(Graphics::eRasterizerState::FrontFaceCulling)] = "FrontFaceCulling";
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

			ImGui::Dummy(ImVec2(0, heightPadding));
			ImGui::AlignTextToFramePadding();
			ImGui::SeparatorText("Render");
			ImGui::Dummy(ImVec2(0, heightPadding));

			static const std::string vSync = std::string("VSync").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(vSync.c_str(), &vsync))
			{
				graphicsGenericDataManager->SetVSync(vsync);
			}

			SameLineDummy(3, 0);

			static const std::string debugLines = std::string("DebugLines").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(debugLines.c_str(), &shouldRenderDebugLines))
			{
				renderer->SetShouldRenderDebugLines(shouldRenderDebugLines);
			}

			SameLineDummy(3, 0);

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

			SameLineDummy(10, 0);

			static const std::string pbr = std::string("PBR").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(pbr.c_str(), &isUsingPBR))
			{
				renderer->SetIsUsingPBR(isUsingPBR);
			}

			SameLineDummy(10, 0);

			static const std::string skeleton = std::string("Skeleton").append(myImGuiTag).c_str();
			if (ImGui::Checkbox(skeleton.c_str(), &shouldRenderSkeleton))
			{
				renderer->SetShouldRenderSkeletonLines(shouldRenderSkeleton);
			}

			SeparatorDummy(0, heightPadding);

			AdjustFPSCap(graphicsEngine);

			ImGui::SetNextItemWidth(200);

			static const std::string rasterizerState = std::string("RasterizerState").append(myImGuiTag).c_str();
			if (ImGui::Combo(rasterizerState.c_str(), &mySelectedRasterizerState, myRasterizerStatesConstChar.data(), static_cast<int>(myRasterizerStatesConstChar.size())))
			{
				graphicsEngine->GetStateManager()->SetRasterizerState(graphicsEngine->GetContext(), static_cast<Graphics::eRasterizerState>(mySelectedRasterizerState));
			}

			SeparatorDummy(0, heightPadding);

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

			SeparatorDummy(0, heightPadding);

			static std::vector<const char*> editorStyles = {"Simple","Dark", "Light" };

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

			SeparatorDummy(0, heightPadding);

			const std::unordered_map<std::string, const HCURSOR>& loadedCursors = Global::GetLoadedCustomCursors();
			std::vector<std::string> cursorNames;
			std::string cursors;

			for (const auto& [name, cursor] : loadedCursors)
			{
				cursorNames.push_back(name);
				cursors += name;
				cursors += '\0';
			}

			cursors += '\0';

			static int selectedCursor = 0;

			const HCURSOR currentCursor = Global::GetCurrentCustomCursor();

			static bool alreadyRunOnce = false;

			if (alreadyRunOnce == false)
			{
				unsigned int index = 0;

				for (const auto& [name, cursor] : loadedCursors)
				{
					if (currentCursor == cursor)
					{
						selectedCursor = index;
						break;
					}

					index++;
				}
			}

			ImGui::SetNextItemWidth(200);

			if (ImGui::Combo("Cursors##SettingWindow", &selectedCursor, cursors.c_str()))
			{
				Global::SetCustomCursor(cursorNames[selectedCursor]);
			}

			SeparatorDummy(0, heightPadding);
		}

		ImGui::End();
	}

	void GraphicsSettingsPopUp::SameLineDummy(float aWidthOffset, float aHeightOffset)
	{
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(aWidthOffset, aHeightOffset));
		ImGui::SameLine();
	}

	void GraphicsSettingsPopUp::SeparatorDummy(float aWidthOffset, float aHeightOffset)
	{
		ImGui::Dummy(ImVec2(aWidthOffset, aHeightOffset));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(aWidthOffset, aHeightOffset));
	}

	void GraphicsSettingsPopUp::AdjustFPSCap(Graphics::GraphicsEngine* aGraphicsEngine)
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
}