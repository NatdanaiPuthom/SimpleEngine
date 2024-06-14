#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class FileMenuBar : public Editor::ToolInterface
	{
	public:
		FileMenuBar();
		~FileMenuBar();

		void Init() override;
		void Update() override;
		void Draw() override;
	private:
	};
}