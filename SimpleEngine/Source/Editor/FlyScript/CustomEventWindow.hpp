#pragma once
#include "NodeScript/FlyDefines.hpp"

namespace Fly
{
	class CustomEventFacade;
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

		void EditInputs(Fly::CustomEventFacade& aCustomEventFacade);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}