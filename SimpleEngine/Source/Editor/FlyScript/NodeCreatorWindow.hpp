#pragma once
#include "NodeScript/SimpleScript/Core/ScriptDefines.hpp"

namespace SCRIPT
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

		void EditInputs(const SCRIPT::CustomEventView& aCustomEventView);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}