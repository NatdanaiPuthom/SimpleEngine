#include "Editor/Precomplied/EditorPch.hpp"
#include "GraphicsSettingsPopUp.hpp"
#include "Engine/Global.hpp"
#include <array>

namespace Editor
{
	GraphicsSettingsPopUp::GraphicsSettingsPopUp(const std::string& aName)
		: PopUp(aName)
		, mySelectedRasterizerState(0)
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

			if (ImGui::Checkbox("VSync##SettingWindow", &vsync))
			{
				graphicsGenericDataManager->SetVSync(vsync);
			}

			SameLineDummy(3, 0);

			if (ImGui::Checkbox("DebugLines##SettingWindow", &shouldRenderDebugLines))
			{
				renderer->SetShouldRenderDebugLines(shouldRenderDebugLines);
			}

			SameLineDummy(3, 0);

			if (ImGui::Checkbox("BoundingBox##SettingWindow", &shouldRenderBoundingBox))
			{
				renderer->SetShouldRenderBoundingBox(shouldRenderBoundingBox);
			}

			if (ImGui::Checkbox("Mesh##SettingWindow", &shouldRenderMesh))
			{
				renderer->SetShouldRenderMesh(shouldRenderMesh);
			}

			SameLineDummy(10, 0);

			if (ImGui::Checkbox("PBR##SettingWindow", &isUsingPBR))
			{
				renderer->SetIsUsingPBR(isUsingPBR);
			}

			SameLineDummy(10, 0);

			if (ImGui::Checkbox("Skeleton##SettingWindow", &shouldRenderSkeleton))
			{
				renderer->SetShouldRenderSkeletonLines(shouldRenderSkeleton);
			}

			ImGui::Dummy(ImVec2(0, heightPadding));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, heightPadding));

			AdjustFPSCap(graphicsEngine);

			ImGui::SetNextItemWidth(200);

			std::array<const char*, static_cast<int>(Graphics::eRasterizerState::Count)> rasterizerStates = {};
			rasterizerStates[static_cast<int>(Graphics::eRasterizerState::BackfaceCulling)] = "BackfaceCulling";
			rasterizerStates[static_cast<int>(Graphics::eRasterizerState::NoFaceCulling)] = "NoFaceCulling";
			rasterizerStates[static_cast<int>(Graphics::eRasterizerState::Wireframe)] = "Wireframe";
			rasterizerStates[static_cast<int>(Graphics::eRasterizerState::WireframeNoCulling)] = "WireframeNoCulling";
			rasterizerStates[static_cast<int>(Graphics::eRasterizerState::FrontFaceCulling)] = "FrontFaceCulling";

			if (ImGui::Combo("RasterizerState##SettingWindow", &mySelectedRasterizerState, rasterizerStates.data(), static_cast<int>(rasterizerStates.size())))
			{
				graphicsEngine->GetStateManager()->SetRasterizerState(graphicsEngine->GetContext(), static_cast<Graphics::eRasterizerState>(mySelectedRasterizerState));
			}
		}

		ImGui::End();
	}

	void GraphicsSettingsPopUp::SameLineDummy(float aWidthOffset, float aHeightOffset)
	{
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(aWidthOffset, aHeightOffset));
		ImGui::SameLine();
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