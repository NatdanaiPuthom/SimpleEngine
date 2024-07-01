#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class PlayMenuBar : public Editor::ToolInterface
	{
	public:
		PlayMenuBar();
		~PlayMenuBar();

		void Init() override;
		void Update() override;
		void Draw() override;
	};
}