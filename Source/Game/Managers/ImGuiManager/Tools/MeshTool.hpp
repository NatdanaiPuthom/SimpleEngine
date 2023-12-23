#pragma once

class Renderer;

class MeshTool final : public ToolInterface
{
public:
	MeshTool(Renderer* aRenderer);

	virtual void Draw() override;
private:
	Renderer* myRenderer;
};
