#pragma once
#include "NodeScript/SimpleScript/Core/FlyDefines.hpp"

namespace Fly
{
	class CustomEventView;
}

namespace Editor
{
	class NodeScriptingWindow;

	class CustomEventWindow final
	{
	public:
		CustomEventWindow(NodeScriptingWindow& aParentWindow);
		~CustomEventWindow();

		void Update();

	private:

		void EditInputs(const Fly::CustomEventView& aCustomEventView);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}