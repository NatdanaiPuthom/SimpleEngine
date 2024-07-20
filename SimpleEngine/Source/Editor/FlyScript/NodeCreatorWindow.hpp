#pragma once
#include "NodeScript/SimpleScript/Core/ScriptDefines.hpp"

namespace Fly
{
	class CustomEventView;
}

namespace Editor
{
	class NodeScriptingWindow;

	class NodeCreatorWindow
	{
	public:
		NodeCreatorWindow(NodeScriptingWindow& aParentWindow);
		~NodeCreatorWindow();

		void Update();

	private:

		void EditInputs(const Fly::CustomEventView& aCustomEventView);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}