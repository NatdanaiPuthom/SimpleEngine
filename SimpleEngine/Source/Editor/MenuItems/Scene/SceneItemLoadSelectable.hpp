#pragma once
#include "Editor/Core/MainMenuItemSelectable.hpp"

namespace Editor
{
	class SceneItemLoadSelectable : public MainMenuItemSelectable
	{
	public:
		SceneItemLoadSelectable(const std::string& aWindowName);

		void Invoke(const char* aText);
	};
}