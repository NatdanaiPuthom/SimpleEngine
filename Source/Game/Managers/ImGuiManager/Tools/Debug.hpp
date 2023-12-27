#pragma once

class DebugTool : public ToolInterface
{
public:
	DebugTool();
	virtual ~DebugTool() override;

	virtual void Draw() override;
};