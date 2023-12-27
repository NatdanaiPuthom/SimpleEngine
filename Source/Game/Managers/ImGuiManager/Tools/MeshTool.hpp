#pragma once

class GameWorld;

class MeshTool final : public ToolInterface
{
public:
	MeshTool();

	virtual void Draw() override;
};
