#pragma once

namespace Tool
{
	class SettingsTool final : public Simple::ToolInterface
	{
	public:
		SettingsTool();

		void Init() override;
		void Draw() override;
	private:
		void ShowDrawCalls();
		void ShowFPS();
	private:
		void ToggleVSync(Simple::GraphicsEngine* aGraphicsEngine);
		void ToggleConsole();
		void ToggleRenderBoundingBox();
		void AdjustWindowSize();
		void AdjustResolution();
		void AdjustRasterizerState();
		void AdjustActiveScene();
		void AdjustFPSCap(Simple::GraphicsEngine* aGraphicsEngine);
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