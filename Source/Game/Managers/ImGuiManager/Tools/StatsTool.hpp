#pragma once

class StatsTool final : public ToolInterface
{
public:
	StatsTool();

	virtual void Draw() override;
private:
	int mySelectedResolution;
	int mySelectedWindowSize;
};
