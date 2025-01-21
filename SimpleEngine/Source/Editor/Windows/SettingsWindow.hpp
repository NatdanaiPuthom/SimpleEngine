#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class SettingsWindow final : public Editor::ToolInterface
	{
	public:
		SettingsWindow();

		void Init() override;
		void Update() override;
		void Draw() override;
	private:
	};
}