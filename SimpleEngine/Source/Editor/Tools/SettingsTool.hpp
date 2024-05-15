#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include "Engine/Math/Vector2.hpp"
#include <string>
#include <vector>

namespace Editor
{
	class SettingsTool final : public Editor::ToolInterface
	{
	public:
		SettingsTool();

		void Init() override;
		void Draw() override;
	private:
		void ShowDrawCalls();
		void ShowFPS();
	private:
		void ToggleVSync(Graphics::GraphicsEngine* aGraphicsEngine);
		void ToggleConsole();
		void AdjustWindowSize();
		void AdjustRasterizerState();
		void AdjustActiveScene();
		void AdjustFPSCap(Graphics::GraphicsEngine* aGraphicsEngine);
	private:
		void UpdateAndFetchCurrentMonitorResolution();
		void LoadDataFromJson();
	private:
		Math::Vector2ui myMonitorResolution;

		int myActiveSceneIndex;
		int mySelectedResolution;
		int mySelectedWindowSize;
		int mySelectedRasterizerState;

		bool myConsoleIsOpen;

		std::vector<Math::Vector2ui> myWindowSizes;
		std::vector<std::string> myScenes;
	};
}