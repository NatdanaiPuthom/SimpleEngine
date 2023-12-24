#pragma once

class Renderer;

class MeshTool final : public ToolInterface
{
public:
	MeshTool();

	virtual void Draw() override;
};
