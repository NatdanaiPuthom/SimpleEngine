#pragma once

namespace Tool
{
	class Settings final : public Simple::ToolInterface
	{
	public:
		Settings();

		virtual void Draw() override;

	private:
		void ShowDrawCalls();
		void ShowFPS();
	private:
		void ToggleConsole();
		void ToggleVSync(Simple::GraphicsEngine* aGraphicsEngine);
		void ToggleMusic();
		void AdjustWindowSize();
		void AdjustResolution();
		void AdjustRasterizerState();
		void AdjustFPSCap(Simple::GraphicsEngine* aGraphicsEngine);
	private:
		void UpdateAndFetchCurrentMonitorResolution();
	private:
		std::vector<SimpleUtilities::Vector2ui> myWindowSizes;

		SimpleUtilities::Vector2ui myMonitorResolution;

		int mySelectedResolution;
		int mySelectedWindowSize;
		int mySelectedRasterizerState;
		bool myConsoleIsOpen;
	};
}