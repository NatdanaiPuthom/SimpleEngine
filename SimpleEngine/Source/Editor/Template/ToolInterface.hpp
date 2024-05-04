#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include <string>

namespace Editor
{
	class ToolInterface
	{
	public:
		virtual ~ToolInterface() = default;

		virtual void Init() {};
		virtual void Update() {};
		virtual void Draw() = 0;

		virtual void LoadSettingsFromJson(const std::string& aFileName = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR)) { aFileName; };

		bool myIsActive = true;
	};
}