#pragma once
#include "SimpleScript/Core/ScriptDefines.h"

namespace SCRIPT
{
	class CustomEventView;
}

namespace Editor
{
	class VisualScriptingWindow;

	class NodeCreatorWindow
	{
	public:
		NodeCreatorWindow(VisualScriptingWindow& aParentWindow);
		~NodeCreatorWindow();

		void Update();

	private:

		void EditInputs(const SCRIPT::CustomEventView& aCustomEventView);

	private:

		VisualScriptingWindow& myParentWindow;
	};
}