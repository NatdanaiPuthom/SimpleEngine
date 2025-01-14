#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class SettingsWindow;
}

namespace Editor
{
	class SettingsMenuBar : public Editor::ToolInterface
	{
	public:
		SettingsMenuBar();
		~SettingsMenuBar();

		void Init() override;
		void Update() override;
		void Draw() override;
		void LoadSettingsFromJson(const std::string& aFileName = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR)) override;

	private:
		std::unique_ptr<SettingsWindow> mySettingWindow;
	};
}