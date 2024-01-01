#pragma once

namespace Tool
{
	class Debug final : public Simple::ToolInterface
	{
	public:
		Debug();
		virtual ~Debug() override;

		virtual void Draw() override;
	private:
		void LoadDataFromJson();
		void WaterPlane();
	private:
		std::vector<int> mySceneIndexes;
		int myActiveScene;
	};
}