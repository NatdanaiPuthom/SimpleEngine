#pragma once

class SettingsTool final : public ToolInterface
{
public:
	SettingsTool();

	virtual void Draw() override;
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
