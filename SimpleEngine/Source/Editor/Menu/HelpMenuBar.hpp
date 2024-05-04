#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class CameraHelpWindow;
}

namespace Editor
{
	class HelpMenuBar : public Editor::ToolInterface
	{
	public:
		HelpMenuBar();
		~HelpMenuBar();

		void Init() override;
		void Update() override;
		void Draw() override;
		void LoadSettingsFromJson(const std::string& aFileName = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR)) override;

	private:
		std::unique_ptr<CameraHelpWindow> myCameraHelpWindow;
	};
}