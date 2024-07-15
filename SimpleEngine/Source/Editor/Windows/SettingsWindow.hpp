#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include "Engine/Math/Vector2.hpp"
#include <string>
#include <vector>
#include <windef.h>

namespace Graphics
{
	class GraphicsEngine;
}

namespace Editor
{
	class SettingsWindow final : public Editor::ToolInterface
	{
	public:
		SettingsWindow();

		void Init() override;
		void Update() override;
		void Draw() override;
	private:
		void ShowDrawCalls();
		void ShowFPS();
		void ShowMusic();
	private:
		void ToggleVSync(Graphics::GraphicsEngine* aGraphicsEngine);
		void ToggleConsole();
		void ToggleUsingPBR(Graphics::GraphicsEngine* aGraphicsEngine);
		void AdjustWindowSize();
		void AdjustRasterizerState();
		void AdjustEditorStyle();
		void AdjustFPSCap(Graphics::GraphicsEngine* aGraphicsEngine);

		void SetCustomCursorIcon();
	private:
		void UpdateAndFetchCurrentMonitorResolution();
		void LoadDataFromJson();

		//NOTE(v11.2.5): Ugly but works for now until future
		void CheckCursorIndexOnce(const std::unordered_map<std::string, const HCURSOR>& aLoadedCursors, int& aSelectedCursor);
	private:
		Math::Vector2ui myMonitorResolution;

		int mySelectedWindowSize;
		int mySelectedRasterizerState;

		bool myConsoleIsOpen;
		bool myMusicIsActive;

		std::vector<Math::Vector2ui> myWindowSizes;
	};
}