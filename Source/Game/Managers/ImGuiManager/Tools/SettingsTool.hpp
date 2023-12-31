#pragma once

class SettingsTool final : public ToolInterface
{
public:
	SettingsTool();

	virtual void Draw() override;

private:
	void ShowDrawCalls();
	void ShowFPS();
private:
	void ToggleConsole();
	void ToggleVSync(GraphicsEngine* aGraphicsEngine);
	void AdjustWindowSize();
	void AdjustResolution();
	void AdjustRasterizerState();
	void AdjustFPSCap(GraphicsEngine* aGraphicsEngine);
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