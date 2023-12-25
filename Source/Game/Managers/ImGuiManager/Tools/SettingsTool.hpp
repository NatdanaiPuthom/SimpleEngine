#pragma once

class SettingsTool final : public ToolInterface
{
public:
	SettingsTool();

	virtual void Draw() override;
private:
	int mySelectedResolution;
	int mySelectedWindowSize;
};
