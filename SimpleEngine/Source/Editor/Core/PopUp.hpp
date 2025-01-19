#pragma once
#include "Editor/Core/Base/Window.hpp"

namespace Editor
{
	class PopUp : public Window
	{
	public:
		PopUp(const std::string& aWindowName);

		virtual void Render() = 0;

		void Update();
	};
}
