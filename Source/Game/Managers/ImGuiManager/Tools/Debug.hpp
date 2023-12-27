#pragma once

class DebugTool final : public ToolInterface
{
public:
	DebugTool();
	virtual ~DebugTool() override;

	virtual void Draw() override;
private:
	void LoadDataFromJson();
private:
	std::vector<int> mySceneIndexes;
	int myActiveScene;
};