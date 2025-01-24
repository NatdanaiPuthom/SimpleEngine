#pragma once
#include "NodeScript/FlyDefines.hpp"

namespace Fly
{
	class CustomEventProxy;
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

		void EditInputs(Fly::CustomEventProxy& aCustomEventProxy);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}