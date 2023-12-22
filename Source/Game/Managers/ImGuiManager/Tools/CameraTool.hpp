#pragma once

class CameraTool : public ToolInterface
{
public:
	CameraTool();

	virtual void Draw() override;
private:
	int mySelectedResolution;
};
