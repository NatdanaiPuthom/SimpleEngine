#pragma once

class DebugTool final : public ToolInterface
{
public:
	DebugTool();
	virtual ~DebugTool() override;

	virtual void Draw() override;
};